package com.hebostary.corejava.concurrence.bank;

import com.hebostary.corejava.*;

public class BankTest {
    private static final int N_ACCOUNTS = 100;
    private static final double INIT_BALANCE = 1000;
    private static final int DELAY = 10;

    public static void Test() {
        Common.PrintClassHeader(new Object(){}.getClass().getEnclosingClass().toString());
        //TestNoSyncBank();
        TestSyncBank();
    }

    /**
     * Test NoSyncBank class
    */
    private static void TestNoSyncBank() {
        Bank bank = new NoSyncBank(N_ACCOUNTS, INIT_BALANCE);
        startMultiTransferThread(bank);

    }

    /**
     * Test SyncBank class
    */
    private static void TestSyncBank() {
        Bank bank = new SyncBank(N_ACCOUNTS, INIT_BALANCE);
        startMultiTransferThread(bank);

    }

    private static void startMultiTransferThread(Bank bank) {
        for (int i = 0; i < bank.size(); i++) {
            int fromAccount = i;
            Runnable r = () -> {
                try {
                    while(true) {
                        int toAccount = (int) (bank.size() * Math.random());
                        //当把amount的数值设置得很大的时候，很容易进入死锁，所有线程都在等待账户有足够的转账金额
                        double amount = INIT_BALANCE * Math.random();
                        bank.transfer(fromAccount, toAccount, amount);
                        Thread.sleep((long) (DELAY * Math.random()));
                    }
                } catch (InterruptedException e) {
                    System.out.println(e);
                }
            };

            Thread t = new Thread(r);
            t.start();
        }
    }
}