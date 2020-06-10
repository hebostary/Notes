package com.hebostary.corejava.generic;

import java.util.Arrays;
import java.util.Collection;

import com.hebostary.corejava.*;

/**
 * 泛型方法
*/
public class GenericMethod {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        Employee[] employees = {
            new Employee("Hunk", 10000),
            new Employee("Grace", 8000),
            new Employee("Jim", 15000),
        };

        //当调用一个泛型方法时，在方法名前的尖括号中放人具体的类型
        ArrayAlg.Pair<Employee> mm = ArrayAlg.<Employee>minmax(employees);
        //大多数情况下，可以省略类型参数<Employee>，等价于：
        //ArrayAlg.Pair<Employee> mm = ArrayAlg.minmax(employees);
        //当程序调用泛型方法时， 如果擦除返回类型， 编译器插入强制类型转换。
        //即，上面的minmax返回的是Object类型，编译器自动插入到Employee的转换

        System.out.println(Arrays.toString(employees));
        //Employee实现Comparable接口时是以salary来进行比较的
        System.out.println("Min: " + mm.getFirst());
        System.out.println("Max: " + mm.getSecond());

        String[] names = {"Hunk", "Jack", "Bob"};
        ArrayAlg.Pair<String> mm1 = ArrayAlg.minmax(names);
        System.out.println(Arrays.toString(names));
        System.out.println("Min: " + mm1.getFirst());
        System.out.println("Max: " + mm1.getSecond());

        //虚拟机中的对象总有一个特定的非泛型类型。因此，所有的类型查询只产生原始类型
        //比如下面的getClass返回的都是相同的Pair class
        System.out.println(mm.getClass().getName());//com.hebostary.corejava.generic.ArrayAlg$Pair
        System.out.println(mm1.getClass().getName());//com.hebostary.corejava.generic.ArrayAlg$Pair

        /*
        Collection<ArrayAlg.Pair<String>> coll = new Set<ArrayAlg.Pair<String>>() ;
        ArrayAlg.Pair<String> pair = new ArrayAlg.Pair<>("Abondon", "Guava");
        ArrayAlg.addAll(coll, mm1, pair);
        System.out.println(coll.toString());
        */
    }

}

class ArrayAlg
{
    /**
     * 静态内部泛型类
    */
    public static class Pair<T>
    {
        private T first;
        private T second;

        //Pair构造器
        public Pair(T f, T s) {
            first = f;
            second = s;
        }

        public T getFirst() {
            return first;
        }

        public T getSecond() {
            return second;
        }
    }

    /**
     * 静态泛型方法
     * 类型参数中的extends对类型参数做了约束：必须实现了Comparable接口，否则会触发编译器错误
     * Comparable接口本身也是一个泛型类型：Comparable<T>
    */
    public static <T extends Comparable<T>> Pair<T> minmax(T[] aTs) {
        if (aTs == null || aTs.length == 0) return null;
        T min = aTs[0];
        T max = aTs[0];
        for (T t : aTs) {
            if (min.compareTo(t) > 0) min = t;
            if (max.compareTo(t) < 0) max = t;
        }
        //返回时可以省略参数类型信息，编译器有足够的信息推导出类型参数
        return new Pair<>(min, max);
    }

    //为了调用这个方法， Java 虚拟机必须建立一个参数化类型的数组，比如Pair<String>数组，这就违反了不能实例化参数化类型数组的规则。
    //对于这种情况，规则有所放松，只会得到下面的警告，而不是错误。
    //Type safety: Potential heap pollution via varargs parameter tsJava(67109670)
    //可以用@SafeVarargs标注来消除创建泛型数组的有关限制
    @SafeVarargs
    public static <T> void addAll(Collection<T> coll, T...ts) {
        for (T t : ts) {
            coll.add(t);
        }
    }
}