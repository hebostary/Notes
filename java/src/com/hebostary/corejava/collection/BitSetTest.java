package com.hebostary.corejava.collection;

import java.util.BitSet;

import com.hebostary.corejava.*;

public class BitSetTest {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());

        BitSet bukSet = new BitSet(7);
        
        //设置指定范围内的位（get将返回true）
        bukSet.set(3, 5);
        System.out.println(bukSet.get(4));

        //清除所有位（get将返回false），也可以清除指定位置
        bukSet.clear();
        System.out.println(bukSet.get(4));
    }
}