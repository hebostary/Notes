package com.hebostary.corejava.collection;

import java.time.LocalDate;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Deque;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.PriorityQueue;
import java.util.Set;
import java.util.TreeSet;

import com.hebostary.corejava.*;

public class CollectionTest {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        LinkedListTest();
        ArrayListTest();
        HashSetTest();
        LinkedHashSetTest();
        TreeSetTest();
        ArrayDequeTest();
        PriorityQueueTest();
    }

    /**
     * 链表，也是一个实现了Queue接口的双端队列
    */
    public static void LinkedListTest() {
        Common.PrintMethodHeader("LinkedListTest");
        List<String> aList = new LinkedList<>();
        aList.add("Amy");
        aList.add("Carl");
        aList.add("Erica");

        List<String> bList = new LinkedList<>();
        bList.add("Bob");
        bList.add("Doug");
        bList.add("Frances");
        bList.add("Gloria");

        //将 bList 的元素合并到 aList，从链表首部交替合并每个元素
        ListIterator<String> aIter = aList.listIterator();
        Iterator<String> bIter = bList.iterator();
        while (bIter.hasNext()) {
            if(aIter.hasNext()) aIter.next();//跳过一个aList中的元素
            aIter.add(bIter.next());
        }

        System.out.println(aList);//[Amy, Bob, Carl, Doug, Erica, Frances, Gloria]

        //删除bList中偶数位置的元素
        bIter = bList.iterator();
        while(bIter.hasNext()){
            bIter.next();//跳过一个元素
            if(bIter.hasNext()) {
                bIter.next();
                bIter.remove();
            }
        }

        System.out.println(bList);//[Bob, Frances]

        //从 aList中删除所有出现在 bList 的单词
        aList.removeAll(bList);
        System.out.println(aList);//[Amy, Carl, Doug, Erica, Gloria]
    }

    /**
     * 数组列表
     * 底层基于数组实现，可以动态扩容
     * get方法支持数组索引随机访问，不支持[]访问
    */
    public static void ArrayListTest() {
        Common.PrintMethodHeader("ArrayListTest");
        ArrayList<String> arrayList = new ArrayList<>();
        arrayList.add("A");
        arrayList.add("B");
        arrayList.add("D");
        arrayList.add("C");
        System.out.println("Print arrayList: " + arrayList.toString());//[A, B, D, C]
        System.out.println("Print arrayList[2]: " + arrayList.get(2));//D
        arrayList.remove(1);
        System.out.println("Print arrayList: " + arrayList.toString());//[A, D, C]
        System.out.println("Print arrayList[2]: " + arrayList.get(2));//C
    }

    /**
     * 散列集 - 无序集合
     * 高效访问，元素不重复
     * 元素无序，只有不关心集合中元素的顺序时才应该使用 HashSet
    */
    public static void HashSetTest() {
        Common.PrintMethodHeader("HashSetTest");
        int capacity = 8;
        float loadFactor = (float)0.75;
        //指定容量和装载因子
        //装填因子（ load factor) 决定何时对散列表进行再散列
        Set<String> hSet = new HashSet<String>(capacity, loadFactor);
        hSet.add("Hunk");
        hSet.add("Jack");
        hSet.add("Bob");
        hSet.add("Grace");
        hSet.add("Hunk");
        System.out.println(hSet.toString());//Hunk, Jack, Bob, Grace,
        System.out.println("Size: " + hSet.size());//4
    
        hSet.add("Bink");
        hSet.add("Jim");
        hSet.add("Sham");
        hSet.add("Provin");
        System.out.println(hSet.toString());//Provin, Bob, Grace, Sham, Hunk, Bink, Jack, Jim,
        //注意：Hunk, Jack, Bob, Grace的顺序已经变化，因为已经重新散列
    }

    /**
     * 链接散列集 - 可以记住元素插入顺序
    */
    public static void LinkedHashSetTest() {
        Common.PrintMethodHeader("LinkedHashSetTest");
        Set<String> hSet = new LinkedHashSet<>();
        hSet.add("Hunk");
        hSet.add("Jack");
        hSet.add("Bob");
        hSet.add("Grace");
        hSet.add("Hunk");
        System.out.println(hSet.toString());//Hunk, Jack, Bob, Grace,
        System.out.println("Size: " + hSet.size());//4
    }

    /**
     * 树集 - 有序集合
     * 基于红黑树实现，元素插入后自动排序
     * 元素类型必须实现Comparable接口
    */
    public static void TreeSetTest() {
        Common.PrintMethodHeader("TreeSetTest");
        Set<String> tSet = new TreeSet<>();
        tSet.add("Hunk");
        tSet.add("Jack");
        tSet.add("Bob");
        tSet.add("Grace");
        tSet.add("Hunk");
        System.out.println(tSet.toString());//[Bob, Grace, Hunk, Jack]
    }

    /**
     * 双端队列，可以让人们有效地在头部和尾部同时添加或删除元素。
     * 不支持在队列中间添加元素。
     * Deque接口，具体实现包括ArrayDeque和LinkedList类
    */
    public static void ArrayDequeTest() {
        Common.PrintMethodHeader("ArrayQequeTest");
        Deque<String> arrDeque = new ArrayDeque<>();
        arrDeque.add("Hunk");
        arrDeque.add("Jack");
        arrDeque.add("Bob");
        arrDeque.add("Grace");
        System.out.println(arrDeque.toString());//[Hunk, Jack, Bob, Grace]
        String first = arrDeque.removeFirst();//删除并返回第一个元素
        String last = arrDeque.removeLast();//删除并返回最后一个元素
        arrDeque.addFirst(last);//在队列首部插入元素
        arrDeque.addLast(first);//在队列尾部插入元素
        System.out.println(arrDeque.toString());//[Grace, Jack, Bob, Hunk]
    }

    /**
     * 优先级队列 -- 基于堆实现
     * 无论何时调用 remove 方法，总会获得当前优先级队列中最小的元素
     * 注意：优先队列的迭代操作并不是按照元素的排列顺序访问的
     * 堆有序并不能保证对二叉树的遍历能得到有序序列
    */
    public static void PriorityQueueTest() {
        Common.PrintMethodHeader("PriorityQueueTest");
        PriorityQueue<LocalDate> pq = new PriorityQueue<>();
        pq.add(LocalDate.of(1996, 6, 23));
        pq.add(LocalDate.of(2006, 3, 3));
        pq.add(LocalDate.of(2003, 8, 21));
        pq.add(LocalDate.of(2004, 8, 21));

        System.out.println("Iterating over elements...");
        for (LocalDate localDate : pq) {
            System.out.println(localDate);
            /* 此时，完全二叉树的结构：
                            1996-06-23
                            /       \
                 2004-08-21          2003-08-21
                 /
           2006-03-03
            根节点在底层数组的索引位置为k=1，左右子结点的位置分别为 2k 和 2k+1
            存储完全二叉树的底层数组：
            [1996-06-23, 2004-08-21, 2003-08-21, 2006-03-03]
            */
        }
        System.out.println("Removing elements...");
        while (!pq.isEmpty()) {
            System.out.println(pq.remove());
            /*
            1996-06-23
            2003-08-21
            2004-08-21
            2006-03-033*/
        }
    }
}