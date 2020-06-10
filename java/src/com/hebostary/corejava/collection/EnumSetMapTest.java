package com.hebostary.corejava.collection;

import java.util.EnumMap;
import java.util.EnumSet;

import com.hebostary.corejava.*;

public class EnumSetMapTest {
    enum Weekday {MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY};

    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        EnumSetTest();
        EnumMapTest();
    }

    //EmimSet 是一个枚举类型元素集的高效实现。 由于枚举类型只有有限个实例， 所以
    //EnumSet 内部用位序列实现。如果对应的值在集中， 则相应的位被置为 1。
    public static void EnumSetTest() {
        Common.PrintMethodHeader("EnumSetTest");
        EnumSet<Weekday> always = EnumSet.allOf(Weekday.class);
        EnumSet<Weekday> never = EnumSet.noneOf(Weekday.class);
        EnumSet<Weekday> workday = EnumSet.range(Weekday.MONDAY, Weekday.FRIDAY);
        System.out.println(always);//[MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY]
        System.out.println(never);//[]
        System.out.println(workday);//[MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY]
    }

    //EnumMap 是一个键类型为枚举类型的映射。它可以直接且高效地用一个值数组实现。
    //在使用时， 需要在构造器中指定键类型
    public static void EnumMapTest() {
        Common.PrintMethodHeader("EnumMapTest");
        EnumMap<Weekday, Employee> personInCharge = new EnumMap<>(Weekday.class);
        personInCharge.put(Weekday.MONDAY, new Employee("Hunk", 1000));
        System.out.println(personInCharge);
    }
}

