package it.sv.ngrams.sequential;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Stream;

public class Application {

  static final String NGRAMS = "ngrams";
  static final String LINES = "lines";

  public static void main(String[] args) throws IOException {

    // map for the input arguments
    Map<String, String> arguments = new HashMap<>();

    for (String arg : args) {
      String[] expressions = arg.split("=");
      arguments.put(expressions[0], expressions[1]);
    }

    // n-grams to extract
    int n = 2;
    if (arguments.containsKey(NGRAMS)) {
      n = Integer.parseInt(arguments.get(NGRAMS));
    }

    Map<String, Integer> ngrams = new HashMap<>();
    Stream<String> stream;

    String pathPart = "10,000";
    if (arguments.containsKey(LINES)) {
      pathPart = arguments.get(LINES);
    }
    String path = System.getProperty("user.dir");
    String directory = null;
    if (path.contains("/ngrams-java/out/production/ngrams-java")) {
      directory = path.replace("/ngrams-java/out/production/ngrams-java", "");
    } else if (path.contains("/ngrams-java")) {
      directory = path.replace("/ngrams-java", "");
    }
    directory += "/done-books/" + pathPart + "_lines/";
    
    long startTime = System.currentTimeMillis();
    System.out.println("Sequential worker start");

    File folder = new File(directory);
    for (final File fileEntry : Objects.requireNonNull(folder.listFiles())) {
      stream = Files.lines(Paths.get(directory + fileEntry.getName()));
      getNgrams(ngrams, n, stream);
      stream.close();
    }

    System.out.println("Sequential worker end");
    long endTime = System.currentTimeMillis();

    long duration = (endTime - startTime);

    System.out.println("Execution time (sequential version): " + duration + " milliseconds");

    AtomicLong count = new AtomicLong();
    ngrams.forEach((key, value) -> count.addAndGet(value));

    System.out.println("count: " + count.toString());
    System.out.println("qu: " + ngrams.get("qu"));

    // System.out.println("\n\n");

    // ngrams.forEach((key, value) -> System.out.println("key: " + key + " - value: " + value));

  }

  /**
   * Static method used to extract the desired n-grams from the current line
   * @param ngrams: map to count the n-grams
   * @param n: n-grams to extract
   * @param stream: contains the current line
   */
  private static void getNgrams(Map<String, Integer> ngrams, int n, Stream<String> stream) {
    stream.forEach(s -> {
      s = s.toLowerCase();
      char[] sToChar = s.toCharArray();
      for (int i = 0; i < s.length() - n + 1; i++) {
        StringBuilder w = new StringBuilder();
        for (int j = 0; j < n; j++) {
          w.append(sToChar[i + j]);
        }
        if (isNgramGood(w.toString())) {
          if(ngrams.get(w.toString()) != null) {
            int partial = ngrams.get(w.toString());
            ngrams.replace(w.toString(), partial + 1);
          } else {
            ngrams.put(w.toString(), 1);
          }
        }
      }
    });
  }

  /**
   * Method used to check if the n-gram is valid
   * @param s n-gram to validate
   * @return (true) if the n-gram is valid, (false) otherwise
   */
  private static boolean isNgramGood(String s) {
    char[] sToChar = s.toCharArray();
    for (char c: sToChar) {
      if (!Character.isLetter(c)) {
        return false;
      }
    }
    return true;
  }

}
