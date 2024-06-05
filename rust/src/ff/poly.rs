use std::ops::{Add, AddAssign, Mul, MulAssign, Neg, Rem, RemAssign, Sub, SubAssign};

use super::field::{Field, FieldRef};

#[derive(PartialEq, Eq, Clone)]
pub struct Poly<F: Field>
where
    for<'a> &'a F: FieldRef<F>,
{
    v: Vec<F>,
}

#[allow(dead_code)]
impl<F: Field> Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    pub fn new(mut v: Vec<F>) -> Self {
        v.reverse();
        Poly { v }
    }

    pub const fn empty() -> Self {
        Poly { v: vec![] }
    }

    pub fn zero() -> Self {
        Poly {
            v: vec![Field::zero()],
        }
    }

    pub fn one() -> Self {
        Poly {
            v: vec![Field::one()],
        }
    }

    pub fn maxdeg(&self) -> usize {
        self.v.len() - 1
    }

    pub fn deg(&self) -> usize {
        for i in (0..self.v.len()).rev() {
            if self.v[i] != F::zero() {
                return i;
            }
        }

        0
    }

    pub fn mindeg(&self) -> usize {
        for i in 0..self.v.len() {
            if self.v[i] != F::zero() {
                return i;
            }
        }

        self.v.len()
    }

    pub fn coeff(&self, i: usize) -> &F {
        &self.v[i]
    }

    pub fn coefficients(&self) -> &Vec<F> {
        &self.v
    }
}

impl<F: Field> AddAssign<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn add_assign(&mut self, rhs: &Poly<F>) {
        let n = self.v.len().max(rhs.v.len());

        self.v.resize(n, F::zero());
        for i in 0..rhs.v.len() {
            self.v[i] += &rhs.v[i];
        }
    }
}

impl<F: Field> AddAssign for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn add_assign(&mut self, rhs: Self) {
        *self += &rhs;
    }
}

impl<F: Field> Add<&Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn add(self, rhs: &Poly<F>) -> Self::Output {
        let mut res = self.clone();
        res += rhs;
        res
    }
}

impl<F: Field> Add<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn add(self, rhs: &Self) -> Self::Output {
        &self + rhs
    }
}

impl<F: Field> Add<Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn add(self, rhs: Poly<F>) -> Self::Output {
        self + &rhs
    }
}

impl<F: Field> Add for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        &self + &rhs
    }
}

impl<F: Field> Neg for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn neg(self) -> Self::Output {
        let mut res = self.clone();

        for x in &mut res.v {
            *x = -x.clone();
        }

        res
    }
}

impl<F: Field> Neg for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn neg(self) -> Self::Output {
        -&self
    }
}

impl<F: Field> SubAssign<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn sub_assign(&mut self, rhs: &Poly<F>) {
        *self += -rhs;
    }
}

impl<F: Field> SubAssign for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn sub_assign(&mut self, rhs: Self) {
        *self -= &rhs;
    }
}

impl<F: Field> Sub<&Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn sub(self, rhs: &Poly<F>) -> Self::Output {
        let mut res = self.clone();
        res -= rhs;
        res
    }
}

impl<F: Field> Sub<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn sub(self, rhs: &Self) -> Self::Output {
        &self - rhs
    }
}

impl<F: Field> Sub<Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn sub(self, rhs: Poly<F>) -> Self::Output {
        self - &rhs
    }
}

impl<F: Field> Sub for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn sub(self, rhs: Self) -> Self::Output {
        &self - &rhs
    }
}

impl<F: Field> MulAssign<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: &Poly<F>) {
        let n = self.v.len() + rhs.v.len() - 1;
        let mut res = vec![Field::zero(); n];

        for i in 0..self.v.len() {
            for j in 0..rhs.v.len() {
                res[i + j] += &self.v[i] * &rhs.v[j];
            }
        }

        self.v = res;
    }
}

impl<F: Field> MulAssign for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: Self) {
        *self *= &rhs;
    }
}

impl<F: Field> Mul<&Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn mul(self, rhs: &Poly<F>) -> Self::Output {
        let mut res: Poly<F> = self.clone();
        res *= rhs;
        res
    }
}

impl<F: Field> Mul<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn mul(self, rhs: &Self) -> Self::Output {
        &self * rhs
    }
}

impl<F: Field> Mul<Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn mul(self, rhs: Poly<F>) -> Self::Output {
        self * &rhs
    }
}

impl<F: Field> Mul for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn mul(self, rhs: Self) -> Self::Output {
        &self * &rhs
    }
}

impl<F: Field> RemAssign<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn rem_assign(&mut self, rhs: &Poly<F>) {
        let d_x = self.deg();
        let d_y = rhs.deg();

        for i in (d_y..=d_x).rev() {
            let t = &self.v[i] / &rhs.v[d_y];
            for j in 0..=d_y {
                self.v[i - j] -= &t * &rhs.v[d_y - j];
            }
        }
    }
}

impl<F: Field> RemAssign for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn rem_assign(&mut self, rhs: Self) {
        *self %= &rhs;
    }
}

impl<F: Field> Rem<&Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn rem(self, rhs: &Poly<F>) -> Self::Output {
        let mut res = self.clone();
        res %= rhs;
        res
    }
}

impl<F: Field> Rem<&Poly<F>> for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn rem(self, rhs: &Self) -> Self::Output {
        &self % rhs
    }
}

impl<F: Field> Rem<Poly<F>> for &Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Poly<F>;

    fn rem(self, rhs: Poly<F>) -> Self::Output {
        self % &rhs
    }
}

impl<F: Field> Rem for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn rem(self, rhs: Self) -> Self::Output {
        &self % &rhs
    }
}

impl<F: Field> std::fmt::Display for Poly<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let mut first = true;
        let zero = F::zero();
        let one = F::one();

        for i in (0..self.v.len()).rev() {
            if self.v[i] == zero {
                continue;
            }

            if !first {
                write!(f, " + ")?;
            }
            first = false;

            if self.v[i] != one || i == 0 {
                if F::deg() == 1 {
                    write!(f, "{}", self.v[i])?;
                } else {
                    write!(f, "({})", self.v[i])?;
                }
            }

            if i > 0 {
                write!(f, "x")?;
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
