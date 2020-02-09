package it.sv.ngrams.parallel;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.BlockingQueue;

class Consumer implements Runnable {
  private int id;
  private int n;
  private boolean canWork;

  // shared data structures
  private MyMap nGrams;
  private BlockingQueue<String> lines;

  // private map
  private Map<String, Integer> localNGrams = new HashMap<>();

  Consumer(int id, int n, BlockingQueue<String> lines, MyMap nGrams) {
    this.id = id;
    this.n = n;
    this.canWork = true;
    this.lines = lines;
    this.nGrams = nGrams;
  }

  public void run() {
    System.out.println("Consumer " + id + " start work");
    while(canWork) {
      if(!lines.isEmpty()) {
        String s = null;
        try {
          s = lines.take();
        } catch (InterruptedException e) {
          e.printStackTrace();
        }
        assert s != null;
        if(s.equals(Application.POISON_PILL)) {
          this.canWork = false;
          nGrams.merge(localNGrams);
          System.out.println("Consumer " + id + " stop work");
        } else {
          processString(s);
        }
      }
    }
  }

  /**
   * Method use to process a line and extract the n-grams
   *
   * @param s line to process
   */
  private void processString(String s) {
    s = s.toLowerCase();
    char[] sToChar = s.toCharArray();
    for (int i = 0; i < s.length() - n + 1; i++) {
      StringBuilder w = new StringBuilder();
      for (int j = 0; j < n; j++) {
        w.append(sToChar[i + j]);
      }
      if (isNgramGood(w.toString())) {
        if (!localNGrams.containsKey(w.toString())) {
          localNGrams.put(w.toString(), 1);
        } else {
          int v = localNGrams.get(w.toString());
          localNGrams.replace(w.toString(), v+1);
        }
      }
    }
  }

  /**
   * Method used to check if the n-gram is valid
   *
   * @param s n-gram to validate
   * @return (true) if the n-gram is valid, (false) otherwise
   */
  private boolean isNgramGood(String s) {
    char[] sToChar = s.toCharArray();
    for (char c: sToChar) {
      if (!Character.isLetter(c)) {
        return false;
      }
    }
    return true;
  }

}
