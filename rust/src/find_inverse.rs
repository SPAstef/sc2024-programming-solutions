mod ff;

use ff::rzp::RZp;
use std::env;

fn main() {
    let args = env::args().collect::<Vec<String>>();

    if args.len() < 3 {
        println!("Usage: {} <x> <p>", args[0]);
        return;
    }

    let p = args[2].parse::<u64>().unwrap();
    RZp::init(p);

    let x = RZp::new(args[1].parse::<u64>().unwrap());

    println!("1/{} = {} (mod {})", x, !x, p);
}
