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
  private int linesNo;

  private Stream<String> stream;
  private BlockingQueue<String> lines;
  private ConcurrentMap<String, Boolean> doneBooks;
  private String pathPart;
  private AtomicInteger activeProducers;

  Producer(int id,
          BlockingQueue<String> lines,
          int consumerNo,
          ConcurrentMap<String, Boolean> doneBooks,
          String pathPart,
          AtomicInteger activeProducers)
  {
    this.id = id;
    this.consumerNo = consumerNo;
    this.lines = lines;
    this.linesNo = 0;
    this.doneBooks = doneBooks;
    this.pathPart = pathPart;
    this.activeProducers = activeProducers;
    this.activeProducers.getAndIncrement();
  }

  public void run() {
    System.out.println("Producer " + id + " start work");
    // String directory = "/Users/stefano/Desktop/done-books/1,000,000_lines/";
    String directory = "/Volumes/Disco Esterno/parallel-v2/done-books/" + pathPart + "_lines/";
    //String directory = "/Users/stefano/Desktop/done-books/" + pathPart + "_lines/";
    File folder = new File(directory);
    for (final File fileEntry : Objects.requireNonNull(folder.listFiles())) {
      if(!doneBooks.containsKey(fileEntry.getName())) {
        doneBooks.putIfAbsent(fileEntry.getName(), true);
        try {
          stream = Files.lines(Paths.get(directory + fileEntry.getName()));
          Object[] line = stream.toArray();
          for (Object token : line) {
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
        } catch (Exception e ){}
      }
    }
    this.activeProducers.getAndDecrement();
    System.out.println("Producer " + id + " stop work");
  }
}