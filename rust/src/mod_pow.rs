mod ff;

use ff::rzp::RZp;
use std::env;

fn main() {
    let args = env::args().collect::<Vec<String>>();

    if args.len() < 4 {
        println!("Usage: {} <x> <y> <p>", args[0]);
        return;
    }

    let p = args[3].parse::<u64>().unwrap();
    RZp::init(p);
    let x = RZp::new(args[1].parse::<u64>().unwrap());
    let y = RZp::new(args[2].parse::<u64>().unwrap());

    println!("{} ^ {} = {} (mod {})", x, y, x ^ y, p);
}
