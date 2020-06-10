package com.hebostary.corejava.exception;

import com.hebostary.corejava.*;

public class ExceptionTest {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        try {
            readData(12);
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    public static String readData(int length) throws FileFormatException
    {
        if (length > 10) {
            throw new FileFormatException("FileFormatException is coming...");
        }

        return new String("Hello");
    }
}