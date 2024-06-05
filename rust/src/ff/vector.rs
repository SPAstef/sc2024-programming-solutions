use std::ops::{
    Add, AddAssign, Div, DivAssign, Index, IndexMut, Mul, MulAssign, Neg, Not, Sub, SubAssign,
};

use std::iter::{IntoIterator, Iterator};

use super::field::{Field, FieldRef};

#[derive(PartialEq, Eq, Clone)]
pub struct Vector<F: Field>
where
    for<'a> &'a F: FieldRef<F>,
{
    v: Vec<F>,
}

#[allow(dead_code)]
impl<F: Field> Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    pub fn new(v: Vec<F>) -> Vector<F> {
        Vector { v }
    }

    pub fn zero(n: usize) -> Vector<F> {
        Vector {
            v: vec![F::zero(); n],
        }
    }

    pub fn len(&self) -> usize {
        self.v.len()
    }

    pub fn circ_mul(&self, circ_mat: &Vector<F>) -> Vector<F> {
        let n = self.len();
        let mut res = Vector::zero(n);

        #[allow(clippy::needless_range_loop)]
        for i in 0..n {
            for j in 0..n {
                res[i] += &self[j] * &circ_mat[(n + i - j) % n];
            }
        }

        res
    }

    pub fn circ_mul_assign(&mut self, circ_mat: &Vector<F>) {
        *self = self.circ_mul(circ_mat);
    }
}

impl<F: Field> Index<usize> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = F;

    fn index(&self, i: usize) -> &Self::Output {
        &self.v[i]
    }
}

impl<F: Field> IndexMut<usize> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn index_mut(&mut self, i: usize) -> &mut Self::Output {
        &mut self.v[i]
    }
}

impl<F: Field> IntoIterator for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Item = F;
    type IntoIter = std::vec::IntoIter<Self::Item>;

    fn into_iter(self) -> Self::IntoIter {
        self.v.into_iter()
    }
}

impl<'a, F: Field> IntoIterator for &'a Vector<F>
where
    for<'b> &'b F: FieldRef<F>,
{
    type Item = &'a F;
    type IntoIter = std::slice::Iter<'a, F>;

    fn into_iter(self) -> Self::IntoIter {
        self.v.iter()
    }
}

impl<'a, F: Field> IntoIterator for &'a mut Vector<F>
where
    for<'b> &'b F: FieldRef<F>,
{
    type Item = &'a mut F;
    type IntoIter = std::slice::IterMut<'a, F>;

    fn into_iter(self) -> Self::IntoIter {
        self.v.iter_mut()
    }
}

impl<F: Field> AddAssign<&Vector<F>> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn add_assign(&mut self, rhs: &Vector<F>) {
        for i in 0..self.len() {
            self[i] += &rhs[i];
        }
    }
}

impl<F: Field> AddAssign for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn add_assign(&mut self, rhs: Vector<F>) {
        *self += &rhs;
    }
}

impl<F: Field> Add<&Vector<F>> for &Vector<F>
where
    for<'b> &'b F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn add(self, rhs: &Vector<F>) -> Self::Output {
        let mut res = self.clone();
        res += rhs;
        res
    }
}

impl<F: Field> Add<&Vector<F>> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn add(self, rhs: &Vector<F>) -> Self::Output {
        &self + rhs
    }
}

impl<F: Field> Add<Vector<F>> for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn add(self, rhs: Vector<F>) -> Self::Output {
        self + &rhs
    }
}

impl<F: Field> Add for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn add(self, rhs: Vector<F>) -> Self::Output {
        &self + &rhs
    }
}

impl<F: Field> Neg for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn neg(self) -> Self::Output {
        let mut res = self.clone();

        for x in &mut res {
            *x = -&*x;
        }

        res
    }
}

impl<F: Field> Neg for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn neg(self) -> Self::Output {
        -&self
    }
}

impl<F: Field> SubAssign<&Vector<F>> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn sub_assign(&mut self, rhs: &Vector<F>) {
        *self += -rhs;
    }
}

impl<F: Field> SubAssign for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn sub_assign(&mut self, rhs: Vector<F>) {
        *self -= &rhs;
    }
}

impl<F: Field> Sub<&Vector<F>> for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn sub(self, rhs: &Vector<F>) -> Self::Output {
        let mut res = self.clone();
        res -= rhs;
        res
    }
}

impl<F: Field> Sub<&Vector<F>> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn sub(self, rhs: &Vector<F>) -> Self::Output {
        &self - rhs
    }
}

impl<F: Field> Sub<Vector<F>> for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn sub(self, rhs: Vector<F>) -> Self::Output {
        self - &rhs
    }
}

impl<F: Field> Sub for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn sub(self, rhs: Vector<F>) -> Self::Output {
        &self - &rhs
    }
}

impl<F: Field> MulAssign<&F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: &F) {
        for x in self {
            *x *= rhs;
        }
    }
}

impl<F: Field> MulAssign<F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: F) {
        *self *= &rhs;
    }
}

impl<F: Field> Mul<&F> for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn mul(self, rhs: &F) -> Self::Output {
        let mut res: Vector<F> = self.clone();
        res *= rhs;
        res
    }
}

impl<F: Field> Mul<F> for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn mul(self, rhs: F) -> Self::Output {
        self * &rhs
    }
}

impl<F: Field> Mul<&F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn mul(self, rhs: &F) -> Self::Output {
        &self * rhs
    }
}

impl<F: Field> Mul<F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn mul(self, rhs: F) -> Self::Output {
        &self * &rhs
    }
}

impl<F: Field> DivAssign<&F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn div_assign(&mut self, rhs: &F) {
        for x in self {
            *x /= rhs;
        }
    }
}

impl<F: Field> DivAssign<F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn div_assign(&mut self, rhs: F) {
        *self /= &rhs;
    }
}

impl<F: Field> Div<&F> for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn div(self, rhs: &F) -> Self::Output {
        let mut res: Vector<F> = self.clone();
        res /= rhs;
        res
    }
}

impl<F: Field> Div<F> for &Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn div(self, rhs: F) -> Self::Output {
        self / &rhs
    }
}

impl<F: Field> Div<&F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Vector<F>;

    fn div(self, rhs: &F) -> Self::Output {
        &self / rhs
    }
}

impl<F: Field> Div<F> for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn div(self, rhs: F) -> Self::Output {
        &self / &rhs
    }
}

impl<F: Field> Not for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn not(self) -> Self::Output {
        let mut res = self.clone();

        for x in &mut res {
            *x = !&*x;
        }

        res
    }
}

impl<F: Field> std::fmt::Display for Vector<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "[")?;

        for (i, x) in self.v.iter().enumerate() {
            if i > 0 {
                write!(f, ", ")?;
            }

            write!(f, "{}", x)?;
        }

        write!(f, "]")
    }
}
