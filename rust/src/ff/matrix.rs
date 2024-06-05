use std::ops::{
    Add, AddAssign, Div, DivAssign, Index, IndexMut, Mul, MulAssign, Neg, Not, Sub, SubAssign,
};

use super::field::{Field, FieldRef};

#[derive(PartialEq, Eq, Clone)]
pub struct Matrix<F: Field>
where
    for<'a> &'a F: FieldRef<F>,
{
    v: Vec<F>,
    rows: usize,
    cols: usize,
}

#[allow(dead_code)]
impl<F: Field> Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    pub fn new(v: Vec<F>, rows: usize, cols: usize) -> Matrix<F> {
        Matrix { v, rows, cols }
    }

    pub fn zero(rows: usize, cols: usize) -> Matrix<F> {
        Matrix {
            v: vec![F::zero()],
            rows,
            cols,
        }
    }

    pub fn rows(&self) -> usize {
        self.rows
    }

    pub fn cols(&self) -> usize {
        self.cols
    }

    pub fn swap(&mut self, i: (usize, usize), j: (usize, usize)) {
        self.v.swap(i.0 * self.cols + i.1, j.0 * self.cols + j.1);
    }

    pub fn transpose_assign(&mut self) {
        for i in 0..self.rows {
            for j in 0..i {
                self.swap((i, j), (j, i));
            }
        }
    }

    pub fn transpose(&self) -> Matrix<F> {
        let mut t = Matrix::zero(self.rows(), self.cols());

        for i in 0..self.rows() {
            for j in 0..self.cols() {
                t[(j, i)] = self[(i, j)].clone();
            }
        }

        t
    }
}

impl<F: Field> Index<(usize, usize)> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = F;

    fn index(&self, i: (usize, usize)) -> &Self::Output {
        &self.v[i.0 * self.cols + i.1]
    }
}

impl<F: Field> IndexMut<(usize, usize)> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn index_mut(&mut self, i: (usize, usize)) -> &mut Self::Output {
        &mut self.v[i.0 * self.cols + i.1]
    }
}

impl<F: Field> AddAssign<&Matrix<F>> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn add_assign(&mut self, rhs: &Matrix<F>) {
        for i in 0..self.v.len() {
            self.v[i] += &rhs.v[i];
        }
    }
}

impl<F: Field> AddAssign for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn add_assign(&mut self, rhs: Self) {
        *self += &rhs;
    }
}

impl<F: Field> Add<&Matrix<F>> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn add(self, rhs: &Matrix<F>) -> Self::Output {
        let mut res = self.clone();
        res += rhs;
        res
    }
}

impl<F: Field> Add<&Matrix<F>> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn add(self, rhs: &Matrix<F>) -> Self::Output {
        &self + rhs
    }
}

impl<F: Field> Add<Matrix<F>> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn add(self, rhs: Matrix<F>) -> Self::Output {
        self + &rhs
    }
}

impl<F: Field> Add for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn add(self, rhs: Matrix<F>) -> Self::Output {
        &self + &rhs
    }
}

impl<F: Field> Neg for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn neg(self) -> Self::Output {
        let mut res = self.clone();

        for x in &mut res.v {
            *x = -&*x;
        }

        res
    }
}

impl<F: Field> Neg for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn neg(self) -> Self::Output {
        -&self
    }
}

impl<F: Field> SubAssign<&Matrix<F>> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn sub_assign(&mut self, rhs: &Matrix<F>) {
        *self += -rhs;
    }
}

impl<F: Field> SubAssign for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn sub_assign(&mut self, rhs: Matrix<F>) {
        *self -= &rhs;
    }
}

impl<F: Field> Sub<&Matrix<F>> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn sub(self, rhs: &Matrix<F>) -> Self::Output {
        let mut res = self.clone();
        res -= rhs;
        res
    }
}

impl<F: Field> Sub<&Matrix<F>> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn sub(self, rhs: &Self) -> Self::Output {
        &self - rhs
    }
}

impl<F: Field> Sub<Matrix<F>> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn sub(self, rhs: Matrix<F>) -> Self::Output {
        self - &rhs
    }
}

impl<F: Field> Sub for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn sub(self, rhs: Self) -> Self::Output {
        &self - &rhs
    }
}

impl<F: Field> MulAssign<&F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: &F) {
        for x in &mut self.v {
            *x *= rhs;
        }
    }
}

impl<F: Field> MulAssign<F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: F) {
        *self *= &rhs;
    }
}

impl<F: Field> Mul<&F> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn mul(self, rhs: &F) -> Self::Output {
        let mut res: Matrix<F> = self.clone();
        res *= rhs;
        res
    }
}

impl<F: Field> Mul<F> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn mul(self, rhs: F) -> Self::Output {
        self * &rhs
    }
}

impl<F: Field> Mul<&F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn mul(self, rhs: &F) -> Self::Output {
        &self * rhs
    }
}

impl<F: Field> Mul<F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn mul(self, rhs: F) -> Self::Output {
        &self * &rhs
    }
}

impl<F: Field> MulAssign<&Matrix<F>> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: &Matrix<F>) {
        let mut res = Matrix::zero(self.rows, rhs.cols);

        for i in 0..self.rows {
            for j in 0..rhs.cols {
                for k in 0..self.cols {
                    res[(i, j)] += &self[(i, k)] * &rhs[(k, j)];
                }
            }
        }

        *self = res;
    }
}

impl<F: Field> MulAssign for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn mul_assign(&mut self, rhs: Self) {
        *self *= &rhs;
    }
}

impl<F: Field> Mul<&Matrix<F>> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn mul(self, rhs: &Matrix<F>) -> Self::Output {
        let mut res = self.clone();
        res *= rhs;
        res
    }
}

impl<F: Field> Mul<&Matrix<F>> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn mul(self, rhs: &Self) -> Self::Output {
        &self * rhs
    }
}

impl<F: Field> Mul<Matrix<F>> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn mul(self, rhs: Matrix<F>) -> Self::Output {
        self * &rhs
    }
}

impl<F: Field> Mul for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn mul(self, rhs: Self) -> Self::Output {
        &self * &rhs
    }
}

impl<F: Field> DivAssign<&F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn div_assign(&mut self, rhs: &F) {
        for x in &mut self.v {
            *x /= rhs;
        }
    }
}

impl<F: Field> DivAssign<F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn div_assign(&mut self, rhs: F) {
        *self /= &rhs;
    }
}

impl<F: Field> Div<&F> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn div(self, rhs: &F) -> Self::Output {
        let mut res: Matrix<F> = self.clone();
        res /= rhs;
        res
    }
}

impl<F: Field> Div<&F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn div(self, rhs: &F) -> Self::Output {
        &self / rhs
    }
}

impl<F: Field> Div<F> for &Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn div(self, rhs: F) -> Self::Output {
        self / &rhs
    }
}

impl<F: Field> Div<F> for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Matrix<F>;

    fn div(self, rhs: F) -> Self::Output {
        &self / &rhs
    }
}

impl<F: Field> Not for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    type Output = Self;

    fn not(self) -> Self::Output {
        let mut res = self.clone();

        for x in &mut res.v {
            *x = !&*x;
        }

        res
    }
}

impl<F: Field> std::fmt::Display for Matrix<F>
where
    for<'a> &'a F: FieldRef<F>,
{
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        for i in 0..self.rows {
            write!(f, "[")?;
            for j in 0..self.cols {
                if j > 0 {
                    write!(f, ", ")?;
                }
                write!(f, "{}", self[(i, j)])?;
            }
            writeln!(f, "]")?;
        }

        Ok(())
    }
}
