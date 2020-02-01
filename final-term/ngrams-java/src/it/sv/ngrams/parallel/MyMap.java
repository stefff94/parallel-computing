package it.sv.ngrams.parallel;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicLong;

public class MyMap {

  private ConcurrentMap<String, Integer> map;

  public MyMap() {
    this.map = new ConcurrentHashMap<>();
  }

  public void addOne(String key) {
    if (!map.containsKey(key)) {
      map.putIfAbsent(key, 1);
    } else {
      int v;
      do {
        v = map.get(key);
      } while (!map.replace(key, v, v+1));
    }
  }

  public int get(String key) {
      return map.get(key);
  }

  public void printReport() {
    AtomicLong count = new AtomicLong();
    map.forEach((key, value) -> count.addAndGet(value));

    System.out.println("count: " + count.toString());

    System.out.println("qu: " + map.get("qu"));
  }

}
