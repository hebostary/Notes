package com.hebostary.corejava.innerClass;

import java.util.Arrays;

import com.hebostary.corejava.*;

public class StaticInnerClass {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        double[] values = new double[20];
        for (int i = 0; i < values.length; i++) {
            values[i] = 100 * Math.random();
        }

        ArrayAlg.Pair p = ArrayAlg.mixmax(values);
        System.out.println(Arrays.toString(values));
        System.out.println("Min: " + p.getFirst());
        System.out.println("Max: " + p.getSecond());

    }
}

class ArrayAlg
{
    /*
        静态内部类
    */
    //有时使用内部类只是为了把一个类隐藏在另外一个类的内部，并不需要内部类引用外围类对象。
    //为此，可以将内部类声明为 static, 以便取消产生的引用。只有内部类可以声明为static
    //静态内部类的对象除了没有对生成它的外围类对象的引用特权外，与其它所有内部类完全一样
    //在内部类不需要访问外部类对象的时候，应该使用静态内部类
    public static class Pair
    {
        private double first;
        private double second;

        //Pair构造器
        public Pair(double f, double s) {
            first = f;
            second = s;
        }

        public double getFirst() {
            return first;
        }

        public double getSecond() {
            return second;
        }
    }

    //提供一个ArrayAlg的静态方法，返回数组中的最大值和最小值
    public static Pair mixmax(double[] values) {
        double min = Double.POSITIVE_INFINITY;
        double max = Double.NEGATIVE_INFINITY;

        for (double d : values) {
            if (d > max) max = d;
            if (d < min) min = d;
        }

        //将最大值和最小值封装在一个Pair中
        return new Pair(min, max);
    }
}