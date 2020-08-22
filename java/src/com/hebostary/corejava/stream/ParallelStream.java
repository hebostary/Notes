package com.hebostary.corejava.stream;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import com.hebostary.corejava.*;

public class ParallelStream {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        String[] strings = {"a","bb","ccc","dddd","eeee","ffffff", "hhhhhhh"};
        //先根据元素长度进行群组，再收集到映射表，这个并行流操作是安全的
        Map<Integer, Long> result = Stream.of(strings).parallel()
            .filter(w -> w.length() < 10)
            .collect(Collectors.groupingBy(String::length, Collectors.counting()));
        System.out.println(result);

        //groupingByConcurrent使用了共享的并发映射表
        Map<Integer, List<String>> rMap = Stream.of(strings).parallel()
            .filter(w -> w.length() < 10)
            .collect(Collectors.groupingByConcurrent(String::length));
        System.out.println(rMap);
    }
}