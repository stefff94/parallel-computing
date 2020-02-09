package it.sv.ngrams.parallel;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicLong;

public class MyMap {

  private ConcurrentMap<String, Integer> map;

  public MyMap() {
    this.map = new ConcurrentHashMap<>();
  }

  /**
   * Method used to merge the private map of a single threads
   *
   * @param mapToMerge map to merge with the private ConcurrentMap map.
   */
  public void merge(Map<String, Integer> mapToMerge) {
    mapToMerge.forEach(this::addValue);
  }

  private void addValue(String key, int value) {
      if (map.putIfAbsent(key, value) != null) {
        int v;
        do {
          v = map.get(key);
        } while (!map.replace(key, v, v+value));
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

  public void printElements() {
    map.forEach( (key, value) -> System.out.println("key: " + key + " - value: " + value));
  }

}
