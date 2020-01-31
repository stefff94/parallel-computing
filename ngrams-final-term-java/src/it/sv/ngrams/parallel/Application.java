package it.sv.ngrams.parallel;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class Application {

  static final String POISON_PILL = "end-of-games";

  static final String NGRAMS = "ngrams";
  static final String PRODUCERS_NO = "producersNo";
  static final String CONSUMERS_NO = "consumersNo";
  static final String LINES = "lines";

  public static void main(String[] args) throws InterruptedException {

    Map<String, String> arguments = new HashMap<>();
    BlockingQueue<String> lines = new ArrayBlockingQueue<>(10000000);
    MyMap ngrams = new MyMap();
    ConcurrentMap<String, Boolean> doneBooks = new ConcurrentHashMap<>();

    for (String arg : args) {
      String[] expressions = arg.split("=");
      arguments.put(expressions[0], expressions[1]);
    }

    // n-grams to extract
    int n = 2;
    if (arguments.containsKey(NGRAMS)) {
      n = Integer.parseInt(arguments.get(NGRAMS));
    }

    // number of producer threads
    int producersNo = 1;
    if (arguments.containsKey(PRODUCERS_NO)) {
      producersNo = Integer.parseInt(arguments.get(PRODUCERS_NO));
    }

    // number of consumer threads
    int consumersNo = 6;
    if (arguments.containsKey(CONSUMERS_NO)) {
      consumersNo = Integer.parseInt(arguments.get(CONSUMERS_NO));
    }

    // elements subdirectory that we want operate with
    String pathPart = "10,000";
    if (arguments.containsKey(LINES)) {
      pathPart = arguments.get(LINES);
    }

    long startTime = System.currentTimeMillis();
    ExecutorService executor = Executors.newFixedThreadPool(producersNo + consumersNo);

    AtomicInteger activeProducers = new AtomicInteger(0);
    for (int i = 0; i < producersNo; ++i) {
      executor.execute(new Producer(i, lines, consumersNo, doneBooks, pathPart, activeProducers));
    }

    for (int i = 0; i < consumersNo; i++) {
      executor.execute(new Consumer(i, n, lines, ngrams));
    }

    executor.shutdown();
    executor.awaitTermination(10L, TimeUnit.MINUTES);

    long endTime = System.currentTimeMillis();

    long duration = (endTime - startTime);

    System.out.println("Execution time (parallel version): " + duration + " milliseconds");

    ngrams.printReport();

  }
}
