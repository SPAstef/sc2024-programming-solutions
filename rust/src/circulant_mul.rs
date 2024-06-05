mod ff;
use ff::rzp::RZp;
use ff::vector::Vector;
use std::env;

fn main() {
    let args = env::args().collect::<Vec<String>>();

    if args.len() < 3 {
        println!("Usage: {} <n> <p>", args[0]);
        return;
    }

    let p = args[2].parse::<u64>().unwrap();
    RZp::init(p);

    let n = args[1].parse::<usize>().unwrap();

    let v = Vector::new((1..=n).map(|x| RZp::new(x as u64)).collect());

    let res = v.circ_mul(&v);

    println!("circ(1, ..., {n}) * {v} = {res} (mod {p})");
}
