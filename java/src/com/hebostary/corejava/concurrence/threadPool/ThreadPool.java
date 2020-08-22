package com.hebostary.corejava.concurrence.threadPool;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ThreadPoolExecutor;

import com.hebostary.corejava.Common;

public class ThreadPool {
    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        try(Scanner in = new Scanner(System.in))
        {
            System.out.print("Enter a directory(eg: /etc): ");
            String dString = in.nextLine();
            System.out.print("Enter a search keyword(eg: java):");
            String keyString = in.nextLine();

            //创建缓冲线程池：必要时创建新线程; 空闲线程会被保留 60 秒
            //ExecutorService pool = Executors.newCachedThreadPool();

            //创建固定数量的线程池,如果提交的任务数多于空闲的线程数，
            //那么把得不到服务的任务放 置到队列中。 当其他任务完成以后再运行它们
            //TODO当把线程数设置得比较大的时候，可以快速完成所有的搜索
            //但是，只是设置10 or 20个线程的时候，线程会卡住
            ExecutorService pool = Executors.newFixedThreadPool(10);

            MatchCounter matchCounter = new MatchCounter(new File(dString), keyString, pool);
            //向线程池提交job，返回一个 Future 对象， 可用来查询该任务的状态
            Future<Integer> result = pool.submit(matchCounter);

            try {
                System.out.println("Matched files number: " + result.get());
            } catch (ExecutionException e) {
                e.printStackTrace();
            }

            //启动该池的关闭序列。被关闭的执 行器不再接受新的任务。当所有任务都完成以后， 线程池中的线程死亡
            pool.shutdown();
            
            int largestPoolSize = ((ThreadPoolExecutor) pool).getLargestPoolSize();
            int maxPoolSize = ((ThreadPoolExecutor) pool).getMaximumPoolSize();
            System.out.println("Max pool size: " + maxPoolSize);
            System.out.println("Largest pool size: " + largestPoolSize);
            
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
}

class MatchCounter implements Callable<Integer> {
    private File directory;
    private String keyword;
    private ExecutorService pool;
    private int count;

    /**
     * 构造匹配搜索对象
    */
    public MatchCounter(File dString, String kString, ExecutorService pool) {
        this.directory = dString;
        this.keyword = kString;
        this.pool = pool;
    }

    /**
     * 实现Callable<Integer>接口，返回一个整数
    */
    public Integer call() {
        count = 0;
        try {
            File[] files = directory.listFiles();
            List<Future<Integer>> results = new ArrayList<>();

            for (File file : files) {
                //为每一个子目录创建新的计算对象并提交到线程池等待运行
                if (file.isDirectory()) {
                    MatchCounter matchCounter = new MatchCounter(file, keyword, pool);
                    Future<Integer> result = pool.submit(matchCounter);
                    results.add(result);
                } else {
                    if(search(file, keyword)) count++;
                    Common.printThreadInfo(Thread.currentThread(), file.toPath().toString());
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
        try {
            try (Scanner in = new Scanner(file)) {
                boolean found = false;
                while (!found && in.hasNextLine()) {
                    if(in.nextLine().contains(keyString))  found = true;
                }
                return found;
            }
        } catch (IOException e) {
            return false;
        }
    }
}