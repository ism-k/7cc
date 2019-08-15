// use std::fs::File; // ファイル操作のためのAPI
// use std::io::{self, BufRead, Write, BufReader}; // 入出力のためのtraitや標準入出力の型
use std::env; // コマンドライン引数

fn main() {

    let args: Vec<String> = env::args().collect();
    let number: i32 = args[1].parse().unwrap();

    println!(".intel_syntax noprefix");
    println!(".global main");
    println!("main:");
    println!("  mov rax, {}", number);
    println!("  ret");
}