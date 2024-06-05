#[derive(Debug)]
struct X {
    x: u64,
}

#[derive(Debug)]
struct Y<'a> {
    y: &'a mut X,
}

struct Z
{
    fn foo()
    {
        t = Y::
        println!("{t:?}");
    }
}

fn main() {
    let mut x = X { x: 42 };
    let y = Y { y: &mut x };

    println!("{y:?}");

    println!()
}
