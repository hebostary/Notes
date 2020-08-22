package com.hebostary.corejava.stream;

import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.TreeSet;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import com.hebostary.corejava.*;

public class CollectingStream {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        collectToSet();
        collectToMap();
        grouping();
        reduce();
    }

    private static void collectToSet() {
        Common.PrintMethodHeader("collectToSet");
        String[] strings = {"a","bb","ccc","dddd","eeeee","ffffff", "hhhhhhh"};

        Stream<String> words = Stream.of(strings);
        //将流的结果收集到一个集合中并打印
        show("words", words.collect(Collectors.toSet()));//words:java.util.HashSet

        words = Stream.of(strings);
        //将流的结果保存到指定的类型集合中
        show("words", words.collect(Collectors.toCollection(TreeSet::new)));//words:java.util.TreeSet

        //将流结果收集到列表
        words = Stream.of(strings);
        List<String> list = words.collect(Collectors.toList());
        System.out.println(list);

        //将流结果收集到数组
        //注意：因为无法在运行时创建泛型数组，因此toArray()返回的是Object[]数组
        //想要得到正确类型的数组，传入指定的数组构造器
        words = Stream.of(strings);
        String[] array = words.toArray(String[]::new);
        System.out.println(array);

        //从流生成一个普通的迭代器
        Iterator<Integer> iter = Stream.iterate(0, n -> n + 1).limit(10).iterator();
        while (iter.hasNext()) {
            System.out.print(iter.next());
        }
        System.out.println();
    }

    private static void collectToMap() {
        Common.PrintMethodHeader("collectToMap");
        Stream<Locale> locales = Stream.of(Locale.getAvailableLocales());
        //默认Locale中的名字为键，而其本地化的名字为值
        //这里我们不关心同一种语言是否会出现两次，比如很多国家都用英语，因此只记录第一项
        Map<String, String> localesMap = locales.collect(Collectors.toMap(Locale::getDisplayLanguage, 
        l -> l.getDisplayLanguage(l), (existingValue, newValue) -> existingValue));
        System.out.println(localesMap.get("Chinese"));
    }

    private static void grouping() {
        Common.PrintMethodHeader("grouping");
        Stream<Locale> locales = Stream.of(Locale.getAvailableLocales());
        //通过群组方法收集给定国家的语言，并输出到映射表中
        //Locale::getCountry是群组的分类函数
        //groupingBy会产生一个映射表，它的每个值都是一个列表
        Map<String, List<Locale>> countryToLocales = locales.collect(
            Collectors.groupingBy(Locale::getCountry));
        System.out.println(countryToLocales.get("CH")); //[gsw_CH, de_CH, pt_CH, fr_CH, rm_CH, it_CH, wae_CH, en_CH]

        //Collectors包提供了很多“下游收集器”方法来获取不同类型的映射表结果
        //群组分类的值是集合而不是默认的列表
        locales = Stream.of(Locale.getAvailableLocales());
        Map<String, Set<Locale>> countryToLocalesSet = locales.collect(
            Collectors.groupingBy(Locale::getCountry, Collectors.toSet()));
        System.out.println(countryToLocalesSet.get("CH")); 

        //群组分类的值是计数
        locales = Stream.of(Locale.getAvailableLocales());
        Map<String, Long> countryToLocalesCount = locales.collect(
            Collectors.groupingBy(Locale::getCountry, Collectors.counting()));
        System.out.println(countryToLocalesCount.get("CH")); //8
    }

    private static void reduce()
    {
        Common.PrintMethodHeader("reduce");
        //计算所有元素的和，参数函数称作累积器（accumulator）
        Stream<Integer> iStream = Stream.iterate(0, n -> n + 1).limit(10);
        Optional<Integer> sum = iStream.reduce((x, y) -> x + y);
        System.out.println(sum.orElse(999));//45等价于将流中的所有元素相加

        //计算所有字符串的长度总和
        //reduce的第一个参数函数叫累积器（accumulator），这个函数会被反复调用，产生累积的总和
        //第二个参数函数叫组合器（combiner），因为并行计算时，会有多个累积器的运算结果，需要通过组合器来合并结果
        String[] words = {"a","bb","ccc","dddd","eeeee","ffffff", "hhhhhhh"};
        int result = Stream.of(words).reduce(0, 
            (total, w) -> total + w.length(), 
            (total1, total2) -> total1 + total2);
        System.out.println("Total length: " + result);//28
        //对于这个特例，使用下面的方法更加高效，因为不涉及装箱操作
        //stream.mapToInt返回一个IntStream，它的元素就是将后面的参数函数应用到原有流的每个元素所得到的结果
        result = Stream.of(words).mapToInt(String::length).sum();
        System.out.println("Total length: " + result);//28
    }

    private static <T> void show(String label, Set<T> set) {
        System.out.println(label + ":" + set.getClass().getName());
        //打印集合元素时，通过流处理来加上分隔符
        System.out.println("[" + set.stream().limit(10)
            .map(Object::toString).collect(Collectors.joining(",")) + "]");
    }
}