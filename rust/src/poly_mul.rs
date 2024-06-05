mod ff;

use ff::poly::Poly;
use ff::rzp::RZp;
use std::env;

fn main() {
    let is_bracket = |c: char| c == '[' || c == ']';

    let args = env::args().collect::<Vec<String>>();

    if args.len() < 5 {
        println!(
            "Usage: {} <p> <[a_*, ..., a_0]> <[b_*, ..., b_0]> <[c_*, ..., c_0]>",
            args[0]
        );
        return;
    }

    let p = args[1].parse::<u64>().unwrap();
    RZp::init(p);

    let a = Poly::new(
        args[2]
            .trim_matches(is_bracket)
            .split(',')
            .map(|x| RZp::new(x.trim().parse::<u64>().unwrap()))
            .collect(),
    );
    let b = Poly::new(
        args[3]
            .trim_matches(is_bracket)
            .split(',')
            .map(|x| RZp::new(x.trim().parse::<u64>().unwrap()))
            .collect(),
    );
    let c = Poly::new(
        args[4]
            .trim_matches(is_bracket)
            .split(',')
            .map(|x| RZp::new(x.trim().parse::<u64>().unwrap()))
            .collect(),
    );

    let r = (&a * &b) % &c;

    println!("({a}) * ({b}) = {r} (mod {c})");
}
