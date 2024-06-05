mod ff;

use ff::poly::Poly;
use ff::rfq::RFq;
use ff::rzp::RZp;
use std::env::args;

fn main() {
    let args = args().collect::<Vec<String>>();
    let is_bracket = |c: char| c == '[' || c == ']';

    if args.len() < 5 {
        println!(
            "Usage: {} <p> <[r_k, ..., r_0]> <[a_*, ..., a_0]>, <b>",
            args[0]
        );
        return;
    }

    let p = args[1].parse::<u64>().unwrap();
    RZp::init(p);

    let r = Poly::new(
        args[2]
            .trim_matches(is_bracket)
            .split(',')
            .map(|x| RZp::new(x.parse::<u64>().unwrap()))
            .collect(),
    );
    RFq::init(r);

    let a = RFq::new(Poly::new(
        args[3]
            .trim_matches(is_bracket)
            .split(',')
            .map(|x| RZp::new(x.parse::<u64>().unwrap()))
            .collect(),
    ));

    let b = args[4].parse::<u64>().unwrap();
    let c = &a ^ b;

    println!("({a})^{b} = {c} (mod {})", RFq::irred())
}
