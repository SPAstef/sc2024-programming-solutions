use super::field::{Field, FieldRef};
use std::ops::{
    Add, AddAssign, BitXor, BitXorAssign, Div, DivAssign, Mul, MulAssign, Neg, Not, Sub, SubAssign,
};

#[derive(Clone, Copy, PartialEq, Eq)]
pub struct RZp {
    x: u64,
}

pub static mut P: u64 = 0;

impl RZp {
    pub fn new(x: u64) -> Self {
        Self { x }
    }

    pub fn init(p: u64) {
        unsafe {
            P = p;
        }
    }

    pub fn val(&self) -> u64 {
        self.x
    }
}

impl<'a> FieldRef<RZp> for &'a RZp {}

impl Field for RZp {
    fn zero() -> Self {
        Self { x: 0 }
    }

    fn one() -> Self {
        Self { x: 1 }
    }

    fn char() -> u64 {
        unsafe { P }
    }

    fn deg() -> usize {
        1
    }

    fn order() -> u64 {
        unsafe { P }
    }
}

impl AddAssign<&RZp> for RZp {
    fn add_assign(&mut self, rhs: &RZp) {
        self.x = self.x.wrapping_add(rhs.x);
        if self.x >= Self::char() || self.x < rhs.x {
            self.x = self.x.wrapping_sub(Self::char());
        }
    }
}

impl AddAssign for RZp {
    fn add_assign(&mut self, rhs: RZp) {
        *self += &rhs;
    }
}

impl Add<&RZp> for &RZp {
    type Output = RZp;

    fn add(self, rhs: &RZp) -> RZp {
        let mut res = *self;
        res += rhs;
        res
    }
}

impl Add<&RZp> for RZp {
    type Output = RZp;

    fn add(self, rhs: &RZp) -> Self::Output {
        &self + rhs
    }
}

impl Add<RZp> for &RZp {
    type Output = RZp;

    fn add(self, rhs: RZp) -> Self::Output {
        self + &rhs
    }
}

impl Add for RZp {
    type Output = RZp;

    fn add(self, rhs: RZp) -> Self::Output {
        &self + &rhs
    }
}

impl Neg for &RZp {
    type Output = RZp;

    fn neg(self) -> Self::Output {
        RZp {
            x: if self.x == 0 { 0 } else { RZp::char() - self.x },
        }
    }
}

impl Neg for RZp {
    type Output = RZp;

    fn neg(self) -> Self::Output {
        -&self
    }
}

impl SubAssign<&RZp> for RZp {
    fn sub_assign(&mut self, rhs: &RZp) {
        *self += -rhs;
    }
}

impl SubAssign for RZp {
    fn sub_assign(&mut self, rhs: RZp) {
        *self -= &rhs;
    }
}

impl Sub<&RZp> for &RZp {
    type Output = RZp;

    fn sub(self, rhs: &RZp) -> RZp {
        let mut res = *self;
        res -= rhs;
        res
    }
}

impl Sub<&RZp> for RZp {
    type Output = RZp;

    fn sub(self, rhs: &RZp) -> Self::Output {
        &self - rhs
    }
}

impl Sub<RZp> for &RZp {
    type Output = RZp;

    fn sub(self, rhs: RZp) -> Self::Output {
        self - &rhs
    }
}

impl Sub for RZp {
    type Output = RZp;

    fn sub(self, rhs: RZp) -> Self::Output {
        &self - &rhs
    }
}

impl MulAssign<&RZp> for RZp {
    fn mul_assign(&mut self, rhs: &RZp) {
        self.x = (((self.x as u128) * (rhs.x as u128)) % (Self::char() as u128)) as u64;
    }
}

impl MulAssign for RZp {
    fn mul_assign(&mut self, rhs: RZp) {
        *self *= &rhs;
    }
}

impl Mul<&RZp> for &RZp {
    type Output = RZp;

    fn mul(self, rhs: &RZp) -> RZp {
        let mut res = *self;
        res *= rhs;
        res
    }
}

impl Mul<&RZp> for RZp {
    type Output = RZp;

    fn mul(self, rhs: &RZp) -> Self::Output {
        &self * rhs
    }
}

impl Mul<RZp> for &RZp {
    type Output = RZp;

    fn mul(self, rhs: RZp) -> Self::Output {
        self * &rhs
    }
}

impl Mul for RZp {
    type Output = RZp;

    fn mul(self, rhs: RZp) -> Self::Output {
        &self * &rhs
    }
}

impl BitXorAssign<u64> for RZp {
    fn bitxor_assign(&mut self, mut rhs: u64) {
        let mut z = RZp::one();
        let mut t = *self;

        while rhs != 0 {
            if (rhs & 1) != 0 {
                z *= t;
            }
            t *= t;

            rhs >>= 1;
        }

        *self = z;
    }
}

impl BitXorAssign<&RZp> for RZp {
    fn bitxor_assign(&mut self, rhs: &RZp) {
        *self ^= rhs.x;
    }
}

impl BitXorAssign for RZp {
    fn bitxor_assign(&mut self, rhs: RZp) {
        *self ^= &rhs;
    }
}

impl BitXor<u64> for &RZp {
    type Output = RZp;

    fn bitxor(self, rhs: u64) -> RZp {
        let mut res = *self;
        res ^= rhs;
        res
    }
}

impl BitXor<u64> for RZp {
    type Output = RZp;

    fn bitxor(self, rhs: u64) -> Self::Output {
        &self ^ rhs
    }
}

impl BitXor<&RZp> for &RZp {
    type Output = RZp;

    fn bitxor(self, rhs: &RZp) -> RZp {
        let mut res = *self;
        res ^= rhs;
        res
    }
}

impl BitXor<&RZp> for RZp {
    type Output = RZp;

    fn bitxor(self, rhs: &RZp) -> Self::Output {
        &self ^ rhs
    }
}

impl BitXor<RZp> for &RZp {
    type Output = RZp;

    fn bitxor(self, rhs: RZp) -> Self::Output {
        self ^ &rhs
    }
}

impl BitXor for RZp {
    type Output = RZp;

    fn bitxor(self, rhs: RZp) -> Self::Output {
        &self ^ &rhs
    }
}

impl Not for &RZp {
    type Output = RZp;

    fn not(self) -> Self::Output {
        let mut res = self.clone();
        res ^= RZp { x: RZp::char() - 2 };
        res
    }
}

impl Not for RZp {
    type Output = RZp;

    fn not(self) -> Self::Output {
        !&self
    }
}

impl DivAssign<&RZp> for RZp {
    fn div_assign(&mut self, rhs: &RZp) {
        *self *= !rhs;
    }
}

impl DivAssign for RZp {
    fn div_assign(&mut self, rhs: RZp) {
        *self /= &rhs;
    }
}

impl Div<&RZp> for &RZp {
    type Output = RZp;

    fn div(self, rhs: &RZp) -> RZp {
        let mut res = self.clone();
        res /= rhs;
        res
    }
}

impl Div<&RZp> for RZp {
    type Output = RZp;

    fn div(self, rhs: &RZp) -> Self::Output {
        &self / rhs
    }
}

impl Div<RZp> for &RZp {
    type Output = RZp;

    fn div(self, rhs: RZp) -> Self::Output {
        self / &rhs
    }
}

impl Div for RZp {
    type Output = RZp;

    fn div(self, rhs: RZp) -> Self::Output {
        &self / &rhs
    }
}

impl std::fmt::Display for RZp {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.x)
    }
}
