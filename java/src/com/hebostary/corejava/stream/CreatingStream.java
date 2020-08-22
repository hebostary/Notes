package com.hebostary.corejava.stream;

import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import com.hebostary.corejava.*;

public class CreatingStream {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        String[] strings = {"a","bb","ccc","dddd","eeeee","ffffff", "hhhhhhh"};

        //从数组创建流
        Stream<String> words = Stream.of(strings);
        words = words.parallel();
        show("words", words);

        //System.out.println(words.max(String::compareToIgnoreCase));//words这个流处理后已经被close掉
        //从数组指定范围元素创建流
        Stream<String> subWords = Arrays.stream(strings, 2, 5).peek(w -> System.out.println("Fetching: " + w));
        show("subWords", subWords);
        Stream<String> song = Stream.of("gently", "down", "the", "stream");
        show("song", song);

        //创建一个空的流
        Stream<String> silence = Stream.empty();
        show("silence", silence);

        //generate函数产生一个无限流，它的元素是通过不断调用传入的参数函数生成的
        Stream<String> echos = Stream.generate(() -> { return "Echo"; });
        show("echos", echos);

        Stream<Double> randoms = Stream.generate(Math::random);
        show("randoms", randoms);

        //iterate函数也是产生无限流，它的元素包含一个种子seed（BigInteger.ONE），以及不断调用f(seed)，
        //然后继续调用f(f(seed))，从而可以产生无限元素
        Stream<BigInteger> integers = Stream.iterate(BigInteger.ONE, n -> n.add(BigInteger.ONE));
        show("integers", integers); 


        //测试filter，map和flatMap从原始流产生一个新的流
        //过滤长度大于3的元素，生成一个新的流
        Stream<String> filterWords = Stream.of(strings).filter(w -> w.length() > 3);
        show("filterWords", filterWords);//filterWords:dddd,eeeee,ffffff,hhhhhhh

        //将所有元素从小写转换成大写，生成一个新的流
        Stream<String> upperWords = Stream.of(strings).map(String::toUpperCase);
        show("upperWords", upperWords);//upperWords:A,BB,CCC,DDDD,EEEEE,FFFFFF,HHHHHHH

        //对words流的每个元素调用letters方法都产生了一个流，
        //因此最终的结果mapWords时一个流中流，也就是每个元素都是一个流
        Stream<Stream<String>> mapWords = Stream.of(strings).map(w -> letters(w));
        show("mapWords", mapWords);
        //而flatMap方法可以把这种流中流摊平合并成一个结果流
        Stream<String> flatMapWords = Stream.of(strings).flatMap(w -> letters(w));
        show("flatMapWords", flatMapWords);//flatMapWords:a,b,b,c,c,c,d,d,d,d,...
    }

    private static <T> void show(String title, Stream<T> stream) {
        final int LIMIT_SIZE = 10;
        List<T> firstElements = stream
            .limit(LIMIT_SIZE + 1)
            .collect(Collectors.toList());

        System.out.print(title + ":");
        for (int i = 0; i < firstElements.size(); i++) {
            if (i > 0) System.out.print(",");
            if (i < LIMIT_SIZE) System.out.print(firstElements.get(i));
            else System.out.print("...");
        }

        System.out.println();
    }

    //将字符串的每个字符作为元素转换成一个流
    private static Stream<String> letters(String s) {
        List<String> result = new ArrayList<>();
        for (int i = 0; i < s.length(); i++) {
            result.add(s.substring(i, i + 1));
        }
        return result.stream();
    }
}