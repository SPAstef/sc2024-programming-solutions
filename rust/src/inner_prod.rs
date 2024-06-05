mod arith;

use arith::poly::Poly;
use arith::zp::RZp;
use std::env;

fn main() {
    let args = env::args().collect::<Vec<String>>();
    let is_bracket = |c: char| c == '[' || c == ']';

    if args.len() < 5 {
        println!(
            "Usage: {} <p> <k> <n> <[[a_0_*, ..., a_0_0]; ...; [a_n_*, ..., a_n_0]]> <[[b_0_*, ..., b_0_0]; ...; [b_n_*, ..., b_n_0]]>", 
            args[0]
        );
        return;
    }

    let p = args[1].parse::<u64>().unwrap();
    let k = args[2].parse::<usize>().unwrap();
    let n = args[3].parse::<usize>().unwrap();

    let a = args[4]
        .trim_matches(is_bracket)
        .split(';')
        .map(|x| {
            Poly::new(
                x.trim_matches(is_bracket)
                    .split(',')
                    .map(|y| RZp::new(y.parse::<u64>().unwrap(), p))
                    .collect::<Vec<RZp>>(),
            )
        })
        .collect::<Vec<Poly>>();

    let b = args[5]
        .trim_matches(is_bracket)
        .split(';')
        .map(|x| {
            Poly::new(
                x.trim_matches(is_bracket)
                    .split(',')
                    .map(|y| RZp::new(y.parse::<u64>().unwrap(), p))
                    .collect::<Vec<RZp>>(),
            )
        })
        .collect::<Vec<Poly>>();

    let ab_dot: Poly = a
        .iter()
        .zip(b.iter())
        .map(|(x, y)| x * y)
        .fold(Poly::zero(p), |acc, x| acc + x);

    println!("{}", ab_dot);
}
