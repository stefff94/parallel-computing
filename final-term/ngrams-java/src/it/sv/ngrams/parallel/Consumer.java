package it.sv.ngrams.parallel;

import java.util.concurrent.BlockingQueue;

class Consumer implements Runnable {
  private boolean canWork;
  private int index;
  private int n;

  private MyMap ngrams;
  private BlockingQueue<String> lines;

  Consumer(int index, int n, BlockingQueue<String> lines, MyMap ngrams) {
    this.index = index;
    this.n = n;
    this.canWork = true;
    this.lines = lines;
    this.ngrams = ngrams;
  }

  public void run() {
    System.out.println("Consumer " + index + " start work");
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

          System.out.println("Consumer " + index + " stop work");
        } else {
          processString(s);
        }
      }
    }
  }

  private void processString(String s) {
    s = s.toLowerCase();
    char[] sToChar = s.toCharArray();
    for (int i = 0; i < s.length() - n + 1; i++) {
      StringBuilder w = new StringBuilder();
      for (int j = 0; j < n; j++) {
        w.append(sToChar[i + j]);
      }

      if (isNgramGood(w.toString())) {
        ngrams.addOne(w.toString());
      }

    }
  }

  private boolean isNgramGood(String s) {
    String rule = " ^[^,.-;:?!«»_'-*()@]+$\"";
    for (int i = 0; i < rule.length(); ++i) {
      if (s.contains(String.valueOf(rule.charAt(i))))
        return false;
    }
    return true;
  }
}
