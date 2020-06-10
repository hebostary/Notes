package com.hebostary.corejava;

import java.util.ArrayList;

public class ArrayListTest {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        //构造一个空的数组列表
        //ArrayList<Employee> staff = new ArrayList<>();
        //等价于ArrayList<Employee> staff = new ArrayList<Employee>();

        //用指定容量构造一个数组列表
        ArrayList<Employee> staff1 = new ArrayList<>(2);
        staff1.add(new Employee("Hunk", 10000));
        staff1.add(new Employee("Bob", 10000));
        System.out.printf("Current address of staff1: 0x%x\n", System.identityHashCode(staff1));
        Common.printAllArrayEleAddr(staff1);

        staff1.add(new Employee("Jack", 10000));
        System.out.printf("Current address of staff1: 0x%x\n", System.identityHashCode(staff1));
        Common.printAllArrayEleAddr(staff1);

        //确保数组列表在不重新分配存储空间的情况下就能够保存给定数量的元素。
        //可能会重新分配存储空间
        staff1.ensureCapacity(10);

        //将数组列表的存储容量削减到当前尺寸。
        //可能会重新分配存储空间，仅在确定数组的大小不会再改变时才使用
        staff1.trimToSize();

//当数组元素的数量超出capacity后，ArrayList会先自动扩容，创建新的底层数组，并拷贝现有数组的所有元素
//实际上底层数组的每个元素都是对对象的引用，因此数组元素引用的实际对象并不会重新分配
        /*
        Current address of staff1: 0xea4a92b
        Current address of index[0]: 0x3c5a99da
        Current address of index[1]: 0x47f37ef1
        Current address of staff1: 0xea4a92b
        Current address of index[0]: 0x3c5a99da
        Current address of index[1]: 0x47f37ef1
        Current address of index[2]: 0x5a01ccaa
        */

        //在指定位置插入元素，后面的所有元素都需要向后移动
        //如果插人新元素后，数组列表的大小超过了容量， 数组列表就会被重新分配存储空间。
        staff1.add(0, new Employee("Grice", 10000));
        //删除指定位置元素，后面的所有元素都需要向前移动
        staff1.remove(0);

//对数组列表实施插人和删除元素的操作其效率比较低。对于小型数组来说， 这一点不必担心。
//但如果数组存储的元素数比较多， 又经常需要在中间位置插入、删除元素，就应该考虑使用链表了。

        //包装器类测试
        Integer a = 127;
        Integer b = 127;
        Integer c = 128;
        Integer d = 128;
        System.out.println((a == b) ? "a equals to b." : "a not equals to b."); //a equals to b.
        System.out.println((c == d) ? "c equals to d." : "c not equals to d."); //c not equals to d.

        System.out.println(Integer.parseInt("12345"));
        System.out.println(Integer.toString(12345));

        //测试变参函数
        System.out.println(ArrayListTest.max(3.1, 300, -43));
    }

    public static double max(double... values)
    {
        double largest = Double.NEGATIVE_INFINITY;
        for (double d : values) {
            if (d > largest) {
                largest = d;
            }
        }
        return largest;
    }
}