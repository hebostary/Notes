package com.hebostary.corejava.generic;

import com.hebostary.corejava.Employee;
import com.hebostary.corejava.Manager;

import com.hebostary.corejava.*;

public class PairTest {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        Manager ceo = new Manager("CEO", 10000, 9000);
        Manager cfo = new Manager("CFO", 8000, 7000);
        Pair<Manager> buddies = new Pair<>(ceo, cfo);
        printBuddies(buddies);

        Manager[] managers = {ceo, cfo};

        Pair<Employee> result = new Pair<>();
        minmaxBonus(managers, result);
        System.out.println("first: "+ result.getFirst().getName() 
            + ", second: " + result.getSecond().getName());

        maxminBonus(managers, result);
        System.out.println("first: "+ result.getFirst().getName()
            + ", second: " + result.getSecond().getName());
    }

    /**
     * 子类型限定的通配符，Pair<? extends Employee>表示任何泛型 Pair 类型， 
     * 它的类型参数是 Employee 的子类， 如 Pair<Manager>， 但不是Pair<String>。
     * 类型 Pair<Manager> 是 Pair<? extends Employee> 的子类型
    */
    public static void printBuddies(Pair<? extends Employee> p) {
        //带有子类型限定的通配符可以从泛型对象读取
        Employee first = p.getFirst();
        Employee second = p.getSecond();
        System.out.println(first.getName() + " and " + second.getName() + " are buddies");
    }

    /**
     * 超类型限定的通配符<? super Manager>
     * 这个通配符限制为 Manager的所有超类型，比如Employee
    */
    public static void minmaxBonus(Manager[] a, Pair<? super Manager> result) {
        if (a.length == 0) return;
        Manager min = a[0];
        Manager max = a[0];
        for (Manager manager : a) {
            if(min.getBonus() > manager.getBonus()) min = manager;
            if(max.getBonus() < manager.getBonus()) max = manager;
        }

        //带有超类型限定的通配符可以向泛型对象写人
        result.setFirst(min);
        result.setSecond(max);
    }

    public static void maxminBonus(Manager[] a, Pair<? super Manager> result) {
        minmaxBonus(a, result);
        PairAlg.swap(result);
    }
}

class Pair<T>
{
    private T first;
    private T second;

    //无参构造器
    public Pair() {
        first = null;
        second = null;
    }

    public Pair(T f, T s) {
        first = f;
        second = s;
    }

    public T getFirst() {
        return first;
    }

    public void setFirst(T f) {
        first = f;
    }

    public T getSecond() {
        return second;
    }

    public void setSecond(T s) {
        second = s;
    }
}

class PairAlg
{
    public static boolean hasNulls(Pair<?> p) {
        return p.getFirst() == null || p.getSecond() == null;
    }

    /**
     * 交换Pair中的两个元素，类型参数使用了通配符，但是swap过程中需要定义一个临时变量，
     * 我们不能用通配符?来作为变量类型，因此实现一个swapHelper来捕获通配符类型
    */
    public static void swap(Pair<?> p) {
        swapHelper(p);
    }

    /**
     * swapHelper捕获通配符类型
     * swapHelper方法的参数 T 捕获通配符。它不知道是哪种类型的通配符， 
     * 但是，这是一个明确的类型
    */
    public static <T> void swapHelper(Pair<T> p) {
        T t = p.getFirst();
        p.setFirst(p.getSecond());
        p.setSecond(t);
    }
}