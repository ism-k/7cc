// use std::fs::File; // ファイル操作のためのAPI
// use std::io::{self, BufRead, Write, BufReader}; // 入出力のためのtraitや標準入出力の型
use std::env; // コマンドライン引数
use std::collections::VecDeque;

enum TokenKind {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
}

// #[derive(Debug)]
struct Token {
    kind: TokenKind,
    value: i32,
    string: String
}
// impl Token {
//     fn new() -> Token {

//     }
// }

fn tokenize(input: String) -> Vec<Token> {
    
    let mut tokens = Vec::new();
    for point in input.as_str().chars() {

    }

    tokens
}

fn main() {

    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        panic!("Invalid number of arguments: {}", args.len());
    }
    let number: i32 = args[1].parse().unwrap();

    println!(".intel_syntax noprefix");
    println!(".global main");
    println!("main:");
    println!("  mov rax, {}", number);

    println!("  ret");
}