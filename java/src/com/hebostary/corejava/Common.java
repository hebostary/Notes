package com.hebostary.corejava;

import java.util.ArrayList;

public class Common {
    public static void PrintHeader(String className)
    {
        System.out.println("************************************");
        System.out.printf("Running %s.Test()\n", className);
        System.out.println("************************************");
    }

    public static void printAllArrayEleAddr(ArrayList arrayList) {
        for (int i = 0; i < arrayList.size(); i++) {
            System.out.printf("Current address of index[%d]: 0x%x\n", i, System.identityHashCode(arrayList.get(i)));
        }
    }
}