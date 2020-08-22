package com.hebostary.corejava.concurrence.future;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.FutureTask;

import com.hebostary.corejava.Common;

public class FutureTest {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        try(Scanner in = new Scanner(System.in))
        {
            System.out.print("Enter a directory(eg: /etc): ");
            String dString = in.nextLine();
            System.out.print("Enter a search keyword(eg: java):");
            String keyString = in.nextLine();
    
            //Future保存异步计算的结果。 可以启动一个计算，将Future对象交给某个线程，然后忘掉它。
            //Future对象的所有者在结果计算好之后就可以获得它。
            //FutureTask包装器是一种非常便利的机制， 可将 Callable 转换成 Future 和 Runnable, 它 同时实现二者的接口
            MatchCounter matchCounter = new MatchCounter(new File(dString), keyString);
            FutureTask<Integer> futureTask = new FutureTask<>(matchCounter);
            Thread t = new Thread(futureTask);
            t.start();
    
            try {
                System.out.println("Matched files number: " + futureTask.get());
            } catch (ExecutionException e) {
                e.printStackTrace();
            }
            
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

class MatchCounter implements Callable<Integer> {
    private File directory;
    private String keyword;

    /**
     * 构造匹配搜索对象
    */
    public MatchCounter(File dString, String kString) {
        this.directory = dString;
        this.keyword = kString;
    }

    /**
     * 实现Callable<Integer>接口，返回一个整数
    */
    public Integer call() {
        int count = 0;
        try {
            File[] files = directory.listFiles();
            List<Future<Integer>> results = new ArrayList<>();

            for (File file : files) {
                //为每一个子目录创建新的future task并放到单独的线程里来完成计算工作
                if (file.isDirectory()) {
                    MatchCounter matchCounter = new MatchCounter(file, keyword);
                    FutureTask<Integer> futureTask = new FutureTask<>(matchCounter);
                    results.add(futureTask);
                    Thread t = new Thread(futureTask);
                    t.start();
                } else {
                    if(search(file, keyword)) count++;
                }
            }

            for (Future<Integer> result : results) {
                try {
                    //每一次对 get 的调用都会发生阻塞直到结果可获得为止
                    count += result.get();
                } catch (ExecutionException e) {
                    e.printStackTrace();
                }
            }
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        return count;
    }

    //查找文件中是否包含某个关键字
    private boolean search(File file, String keyString) {
        try (Scanner in = new Scanner(file)){
            boolean found = false;
            while (!found && in.hasNextLine()) {
                if(in.nextLine().contains(keyString)) {
                    found = true;
                }
            }

            Common.printThreadInfo(Thread.currentThread(), file.toPath().toString());
            return found;
        } catch (IOException e) {
            return false;
        }
    }
}