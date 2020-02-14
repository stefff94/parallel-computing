package it.sv.ngrams.parallel;

import java.io.File;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Objects;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Stream;

class Producer implements Runnable {
  private int id;
  private int consumerNo;
  private int linesNo = 0;

  private Stream<String> stream;
  private String path;

  // shared data structures
  private BlockingQueue<String> lines;
  private ConcurrentMap<String, Boolean> doneBooks;

  private AtomicInteger activeProducers;

  Producer(int id,
          BlockingQueue<String> lines,
          int consumerNo,
          ConcurrentMap<String, Boolean> doneBooks,
          String path,
          AtomicInteger activeProducers)
  {
    this.id = id;
    this.consumerNo = consumerNo;
    this.lines = lines;
    this.doneBooks = doneBooks;
    this.path = path;
    this.activeProducers = activeProducers;
    this.activeProducers.getAndIncrement();
  }

  public void run() {
    System.out.println("Producer " + id + " start work");
    
    // String directory = "../done-books/" + pathPart + "_lines/";
    String directory = path;
    File folder = new File(directory);
    for (final File fileEntry : Objects.requireNonNull(folder.listFiles())) {
      if(doneBooks.putIfAbsent(fileEntry.getName(), true) == null) {
        try {
          stream = Files.lines(Paths.get(directory + fileEntry.getName()));
          for (Object token : stream.toArray()) {
            lines.put(token.toString());
            ++ linesNo;
          }
        } catch (Exception e) {
          e.printStackTrace();
        } finally {
          stream.close();
        }
      }
    }
    System.out.println("Producer " + id +  " added " + linesNo + " lines");

    if(this.activeProducers.get() == 1) {
      for(int i = 0; i < consumerNo; i++) {
        try {
          lines.put(Application.POISON_PILL);
        } catch (Exception e ){
          e.printStackTrace();
        }
      }
    }
    this.activeProducers.getAndDecrement();
    System.out.println("Producer " + id + " stop work");
  }
}