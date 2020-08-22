package com.hebostary.corejava.concurrence.bank;

import java.util.Arrays;

public class Bank {
    private final double[] accounts;

    /**
     * Construct a bank object
     * @param n
     * @param initialBalance
    */
    public Bank(int n, double initialBalance) {
        accounts = new double[n];
        Arrays.fill(accounts, initialBalance);
    }

    public double getAccount(int n) {
        if(n > size()-1) return Double.MIN_VALUE;
        return accounts[n];
    }

    public void setAccount(int n, double balance) {
        if(n > size()-1) return;
        accounts[n] = balance;
    }

    /**
     * Transfer amount from one account to another account
    */
    public void transfer(int from, int to, double amount) {}

    /**
     * Return total balance of all accounts
     * @return
    */
    public double getTotalBalance() { return 0;}

    /**
     * Return the size of accounts
    */
    public int size() {
        return accounts.length;
    }
}