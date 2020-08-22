package com.hebostary.corejava.concurrence.bank;

import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

import com.hebostary.corejava.*;

public class SyncBank extends Bank {
    private ReentrantReadWriteLock rWriteLock = new ReentrantReadWriteLock();
    //得到一个可以被多个读操作共用的读锁， 但会排斥所有写操作。
    private Lock bankReadLock = rWriteLock.readLock();
    //得到一个写锁， 排斥所有其他的读操作和写操作。
    private Lock bankWriteLock = rWriteLock.writeLock();
    //得到一个条件对象
    private Condition enoughAccountCon = bankWriteLock.newCondition();

    /**
     * Construct a SyncBank object
     * @param n
     * @param initialBalance
    */
    public SyncBank(int n, double initialBalance) {
        super(n, initialBalance);
    }

    /**
     * Transfer amount from one account to another account
    */
    public void transfer(int from, int to, double amount) {
        //进入临界区首先获得锁，阻止竞争条件发生
        bankWriteLock.lock();
        try {
            while (getAccount(from) < amount) {
                //获得锁后，可能发现账户金额不足，调用await进人该条件的等待集。
                //此时，该线程放弃了锁，使得其他线程可以继续完成转账操作
                //注意，等待获得锁的线程和调用await的线程存在着本质上的不同：
                //一旦一个线程调用 await 方法,当锁可用时,该线程不能马上解除阻塞。 
                //相反， 它处于阻塞状态,直到另一个线程调用同一条件上的 signalAll 方法时为止
                enoughAccountCon.await();

                //当然，在实际中，我们可能更倾向于直接返回错误，而不是等待账户有足够的金额
            }
            Common.printThreadInfo(Thread.currentThread(), "Hi");
            setAccount(from, getAccount(from)-amount);
            System.out.printf("[INFO] %10.2f from %d to %d\n", amount, from, to);
            setAccount(to, getAccount(to)+amount);
            System.out.printf("[INFO] Total balance: %10.2f\n", getTotalBalance());

            //重新激活因为这一条件而等待的所有线程
            enoughAccountCon.signalAll();
        } catch ( InterruptedException e ) {
            System.out.println(e);
        } finally {
            //把解锁操作括在 finally 子句之内是至关重要的。 
            //如果在临界区的代码抛出异常，锁必须被释放。否则，其他线程将永远阻塞。
            bankWriteLock.unlock();
        }
    }

    /**
     * Return total balance of all accounts
     * @return
    */
    public double getTotalBalance() {
        double total = 0.0;
        bankReadLock.lock();
        try {
            for (int i = 0; i < size(); i++) {
                total += getAccount(i);
            }
        } finally {
            bankReadLock.unlock();
        }

        return total;
    }
}