// use std::fs::File; // ファイル操作のためのAPI
// use std::io::{self, BufRead, Write, BufReader}; // 入出力のためのtraitや標準入出力の型
use std::env; // コマンドライン引数
use std::collections::VecDeque;

#[derive(Debug)]
enum TokenKind {
    TK_RESERVED, // 記号
    TK_NUM,      // 整数トークン
    TK_EOF,      // 入力の終わりを表すトークン
}

#[derive(Debug)]
struct Token {
    kind: TokenKind,
    value: i32,
    string: String
}
impl Token {
    // コンストラクタ
    fn new(kind: TokenKind, value: i32, string: String) -> Token {
        Token {
            kind: kind,
            value: value,
            string: string
        }
    }
}

// トークンが期待する演算子ならtrue，それ以外はfalse
fn expect_operator(operator: char, token: &Token) -> bool {
    match token.kind {
        TokenKind::TK_RESERVED => token.string == operator.to_string(),
        TokenKind::TK_NUM      => false,
        TokenKind::TK_EOF      => false,
    }
}

// トークンが数値ならその値を，それ以外ならNone
fn expect_number(token: &Token) -> Option<i32> {
    match token.kind {
        TokenKind::TK_NUM      => Some(token.value),
        TokenKind::TK_EOF      => None,
        TokenKind::TK_RESERVED => None,
    }
}

fn tokenize(input: &mut String) -> VecDeque<Token> {
    
    // tokenを格納するvectorを作成
    let mut tokens = VecDeque::new();
    // 入力文字列から空白を除去
    input.retain(|c| c != ' ');
    // 入力文字列からcharの配列を作成
    let points: Vec<char> = input.chars().collect::<Vec<char>>();

    let mut sum: u32 = 0;
    let mut iter: usize = 0;
    while iter < points.len() {
        // println!("check char: {}", points[iter]);
        // + か - は記号
        if points[iter] == '+' || points[iter] == '-' {
            let token: Token = Token::new(TokenKind::TK_RESERVED, 0, points[iter].to_string());
            // println!("  operator: {:?}", token);
            tokens.push_back(token);
            iter += 1;
            continue;
        }
        // 数字を見つけたら足し続ける
        if points[iter].is_digit(10) {
            sum += points[iter].to_digit(10).unwrap();

            // これで文字列が最後ならトークン作成
            if iter == points.len()-1 {
                let token: Token = Token::new(TokenKind::TK_NUM, sum as i32, sum.to_string());
                // println!("  number: {:?}", token);
                tokens.push_back(token);
                sum = 0;
            }
            // 次の文字が数値でないならトークン作成
            else if !points[iter+1].is_digit(10) {
                let token: Token = Token::new(TokenKind::TK_NUM, sum as i32, sum.to_string());
                // println!("  number: {:?}", token);
                tokens.push_back(token);
                sum = 0;
            }
            // 次の文字も数値なら現在までの値を10倍しておく
            else {
                sum *= 10;
            }

            iter += 1;
            continue;
        }

        panic!("Error: tokenize unavailable");
    }

    tokens
}

fn main() {

    let mut args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        panic!("Error: Invalid number of arguments {}", args.len());
    }
    // let number: i32 = args[1].parse().unwrap();
    // let mut operators = args[1];
    let mut tokens: VecDeque<Token> = tokenize(&mut args[1]);
    // println!("{:?}", tokens);

    println!(".intel_syntax noprefix");
    println!(".global main");
    println!("main:");

    // 式の最初が数値であるか確認
    match expect_number(&tokens[0]) {
        Some(result) => println!("  mov rax, {}", result),
        None         => panic!("Error: first term of expression must be numeric"),
    }
    tokens.pop_front();

    while !tokens.is_empty() {
        let token: Token = tokens.pop_front().unwrap();

        if expect_operator('+', &token) {
            let token_number: Token = tokens.pop_front().unwrap();
            match expect_number(&token_number) {
                Some(result) => println!("  add rax, {}", result),
                None         => panic!("Error: the term after operator {} must be numeric", '+'),
            }
            continue;
        }
        if expect_operator('-', &token) {
            let token_number: Token = tokens.pop_front().unwrap();
            match expect_number(&token_number) {
                Some(result) => println!("  sub rax, {}", result),
                None         => panic!("Error: the term after operator {} must be numeric", '-'),
            }
            continue;
        }
    }

    println!("  ret");
}