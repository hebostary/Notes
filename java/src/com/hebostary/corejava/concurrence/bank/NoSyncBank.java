package com.hebostary.corejava.concurrence.bank;

import com.hebostary.corejava.Common;

public class NoSyncBank extends Bank {
    /**
     * Construct a NoSyncBank object
     * @param n
     * @param initialBalance
    */
    public NoSyncBank(int n, double initialBalance) {
        super(n, initialBalance);
    }

    /**
     * Transfer amount from one account to another account
    */
    public void transfer(int from, int to, double amount) {
        if (getAccount(from) < amount) return;
        Common.printThreadInfo(Thread.currentThread(), "Hi");
        setAccount(from, getAccount(from)-amount);
        System.out.printf("[INFO] %10.2f from %d to %d\n", amount, from, to);
        setAccount(to, getAccount(to)+amount);
        System.out.printf("[INFO] Total balance: %10.2f\n", getTotalBalance());
    }

    /**
     * Return total balance of all accounts
     * @return
    */
    public double getTotalBalance() {
        double total = 0.0;
        for (int i = 0; i < size(); i++) {
            total += getAccount(i);
        }
        return total;
    }
}