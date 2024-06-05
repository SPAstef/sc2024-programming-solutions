use super::field::{Field, FieldRef};
use super::poly::Poly;
use super::rzp::RZp;

use std::ops::{
    Add, AddAssign, BitXor, BitXorAssign, Div, DivAssign, Mul, MulAssign, Neg, Not, Sub, SubAssign,
};

#[derive(PartialEq, Eq, Clone)]
pub struct RFq {
    x: Poly<RZp>,
}

pub static mut R: Poly<RZp> = Poly::empty();

impl RFq {
    pub fn new(x: Poly<RZp>) -> Self {
        Self { x }
    }

    pub fn init(r: Poly<RZp>) {
        unsafe {
            R = r;
        }
    }

    pub fn irred() -> &'static Poly<RZp> {
        unsafe { &R }
    }
}

impl<'a> FieldRef<RFq> for &'a RFq {}

impl Field for RFq {
    fn zero() -> Self {
        Self { x: Poly::zero() }
    }

    fn one() -> Self {
        Self { x: Poly::one() }
    }

    fn char() -> u64 {
        RZp::char()
    }

    fn deg() -> usize {
        unsafe { R.deg() }
    }

    fn order() -> u64 {
        RFq::char().pow(RFq::deg() as u32)
    }
}

impl AddAssign<&RFq> for RFq {
    fn add_assign(&mut self, rhs: &RFq) {
        self.x += &rhs.x;
    }
}

impl AddAssign for RFq {
    fn add_assign(&mut self, rhs: Self) {
        *self += &rhs;
    }
}

impl Add<&RFq> for &RFq {
    type Output = RFq;

    fn add(self, rhs: &RFq) -> Self::Output {
        let mut res = self.clone();
        res += rhs;
        res
    }
}

impl Add<&RFq> for RFq {
    type Output = RFq;

    fn add(self, rhs: &RFq) -> Self::Output {
        &self + rhs
    }
}

impl Add<RFq> for &RFq {
    type Output = RFq;

    fn add(self, rhs: RFq) -> Self::Output {
        self + &rhs
    }
}

impl Add for RFq {
    type Output = RFq;

    fn add(self, rhs: RFq) -> Self::Output {
        &self + &rhs
    }
}

impl Neg for &RFq {
    type Output = RFq;

    fn neg(self) -> Self::Output {
        RFq { x: -&self.x }
    }
}

impl Neg for RFq {
    type Output = RFq;

    fn neg(self) -> Self::Output {
        -&self
    }
}

impl SubAssign<&RFq> for RFq {
    fn sub_assign(&mut self, rhs: &RFq) {
        *self += -rhs;
    }
}

impl SubAssign for RFq {
    fn sub_assign(&mut self, rhs: Self) {
        *self -= &rhs;
    }
}

impl Sub<&RFq> for &RFq {
    type Output = RFq;

    fn sub(self, rhs: &RFq) -> Self::Output {
        let mut res = self.clone();
        res -= rhs;
        res
    }
}

impl Sub<&RFq> for RFq {
    type Output = RFq;

    fn sub(self, rhs: &RFq) -> Self::Output {
        &self - rhs
    }
}

impl Sub<RFq> for &RFq {
    type Output = RFq;

    fn sub(self, rhs: RFq) -> Self::Output {
        self - &rhs
    }
}

impl Sub for RFq {
    type Output = RFq;

    fn sub(self, rhs: RFq) -> Self::Output {
        &self - &rhs
    }
}

impl MulAssign<&RFq> for RFq {
    fn mul_assign(&mut self, rhs: &RFq) {
        self.x *= &rhs.x;
        self.x %= RFq::irred();
    }
}

impl MulAssign for RFq {
    fn mul_assign(&mut self, rhs: Self) {
        *self *= &rhs;
    }
}

impl Mul<&RFq> for &RFq {
    type Output = RFq;

    fn mul(self, rhs: &RFq) -> Self::Output {
        let mut res = self.clone();
        res *= rhs;
        res
    }
}

impl Mul<&RFq> for RFq {
    type Output = RFq;

    fn mul(self, rhs: &RFq) -> Self::Output {
        &self * rhs
    }
}

impl Mul<RFq> for &RFq {
    type Output = RFq;

    fn mul(self, rhs: RFq) -> Self::Output {
        self * &rhs
    }
}

impl Mul for RFq {
    type Output = RFq;

    fn mul(self, rhs: RFq) -> Self::Output {
        &self * &rhs
    }
}

impl BitXorAssign<u64> for RFq {
    // We overload the `^` operator to mean exponentiation.
    fn bitxor_assign(&mut self, mut rhs: u64) {
        let mut res = RFq::one();

        while rhs != 0 {
            if rhs & 1 == 1 {
                res *= &*self;
            }

            *self = &*self * &*self;
            rhs >>= 1;
        }
        *self = res;
    }
}

impl BitXor<u64> for &RFq {
    type Output = RFq;

    fn bitxor(self, rhs: u64) -> Self::Output {
        let mut res = self.clone();
        res ^= rhs;
        res
    }
}

impl BitXor<u64> for RFq {
    type Output = RFq;

    fn bitxor(self, rhs: u64) -> Self::Output {
        &self ^ rhs
    }
}

impl Not for &RFq {
    type Output = RFq;

    fn not(self) -> Self::Output {
        let mut res = self.clone();
        res ^= RFq::order() - 2;
        res
    }
}

impl Not for RFq {
    type Output = RFq;

    fn not(self) -> Self::Output {
        !&self
    }
}

impl DivAssign<&RFq> for RFq {
    fn div_assign(&mut self, rhs: &RFq) {
        *self *= !rhs;
    }
}

impl DivAssign for RFq {
    fn div_assign(&mut self, rhs: Self) {
        *self /= &rhs;
    }
}

impl Div<&RFq> for &RFq {
    type Output = RFq;

    fn div(self, rhs: &RFq) -> Self::Output {
        let mut res = self.clone();
        res /= rhs;
        res
    }
}

impl Div<&RFq> for RFq {
    type Output = RFq;

    fn div(self, rhs: &RFq) -> Self::Output {
        &self / rhs
    }
}

impl Div<RFq> for &RFq {
    type Output = RFq;

    fn div(self, rhs: RFq) -> Self::Output {
        self / &rhs
    }
}

impl Div for RFq {
    type Output = RFq;

    fn div(self, rhs: RFq) -> Self::Output {
        &self / &rhs
    }
}

impl std::fmt::Display for RFq {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let mut first = true;

        for i in (0..self.x.coefficients().len()).rev() {
            if *self.x.coeff(i) == RZp::zero() {
                continue;
            }

            if !first {
                write!(f, " + ")?;
            }
            first = false;

            if *self.x.coeff(i) != RZp::one() || i == 0 {
                write!(f, "{}", self.x.coeff(i))?;
            }

            if i > 0 {
                write!(f, "a")?;
                if i > 1 {
                    write!(f, "^{}", i)?;
                }
            }
        }

        if first {
            write!(f, "0")?;
        }

        Ok(())
    }
}
