package com.hebostary.corejava.exception;

import java.io.IOException;

class FileFormatException extends IOException {
    //习惯上，所有派生的异常类都支持一个默认的构造器和一个带有详细描述信息的构造器。
    public FileFormatException() {}
    public FileFormatException(String grige)
    {
        super(grige);
    }
}