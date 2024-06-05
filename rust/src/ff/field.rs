use std::{
    fmt::Display,
    ops::{
        Add, AddAssign, BitXor, BitXorAssign, Div, DivAssign, Mul, MulAssign, Neg, Not, Sub,
        SubAssign,
    },
};

pub trait FieldRef<NonRef>:
    Sized
    + Add<NonRef, Output = NonRef>
    + Sub<NonRef, Output = NonRef>
    + Neg<Output = NonRef>
    + Mul<NonRef, Output = NonRef>
    + BitXor<u64, Output = NonRef>
    + Not<Output = NonRef>
    + Div<NonRef, Output = NonRef>
    + for<'b> Add<&'b NonRef, Output = NonRef>
    + for<'b> Sub<&'b NonRef, Output = NonRef>
    + Neg<Output = NonRef>
    + for<'b> Mul<&'b NonRef, Output = NonRef>
    + Not<Output = NonRef>
    + for<'b> Div<&'b NonRef, Output = NonRef>
{
}

pub trait Field:
    Sized
    + Clone
    + Display
    + PartialEq
    + Eq
    + Add<Self, Output = Self>
    + Neg<Output = Self>
    + Sub<Self, Output = Self>
    + Mul<Self, Output = Self>
    + BitXor<u64, Output = Self>
    + Not<Output = Self>
    + Div<Self, Output = Self>
    + AddAssign<Self>
    + SubAssign<Self>
    + MulAssign<Self>
    + BitXorAssign<u64>
    + DivAssign<Self>
    + for<'a> Add<&'a Self, Output = Self>
    + for<'a> Sub<&'a Self, Output = Self>
    + for<'a> Neg<Output = Self>
    + for<'a> Mul<&'a Self, Output = Self>
    + for<'a> Not<Output = Self>
    + for<'a> Div<&'a Self, Output = Self>
    + for<'a> AddAssign<&'a Self>
    + for<'a> SubAssign<&'a Self>
    + for<'a> MulAssign<&'a Self>
    + for<'a> DivAssign<&'a Self>
where
    for<'a> &'a Self: FieldRef<Self>,
{
    fn zero() -> Self;
    fn one() -> Self;
    fn char() -> u64;
    fn deg() -> usize;
    fn order() -> u64;
}

/*
//This causes infinite recursion, where "FieldValue" is supposed to extract the contents of Field

trait FieldValue:
    Sized
    + Clone
    + Display
    + PartialEq
    + Eq
    + Add<Self, Output = Self>
    + Neg<Output = Self>
    + Sub<Self, Output = Self>
    + Mul<Self, Output = Self>
    + BitXor<Self, Output = Self>
    + Not<Output = Self>
    + Div<Self, Output = Self>
    + AddAssign<Self>
    + SubAssign<Self>
    + MulAssign<Self>
    + BitXorAssign<Self>
    + DivAssign<Self>
    + for<'a> Add<&'a Self, Output = Self>
    + for<'a> Sub<&'a Self, Output = Self>
    + for<'a> Neg<Output = Self>
    + for<'a> Mul<&'a Self, Output = Self>
    + for<'a> BitXor<&'a Self, Output = Self>
    + for<'a> Not<Output = Self>
    + for<'a> Div<&'a Self, Output = Self>
    + for<'a> AddAssign<&'a Self>
    + for<'a> SubAssign<&'a Self>
    + for<'a> MulAssign<&'a Self>
    + for<'a> BitXorAssign<&'a Self>
    + for<'a> DivAssign<&'a Self>
{
}

impl<'a, T> FieldRef<T> for &'a T
where
    T: FieldValue,
    &'a T: Sized
        + Add<T, Output = T>
        + Sub<T, Output = T>
        + Neg<Output = T>
        + Mul<T, Output = T>
        + BitXor<T, Output = T>
        + Not<Output = T>
        + Div<T, Output = T>
        + for<'b> Add<&'b T, Output = T>
        + for<'b> Sub<&'b T, Output = T>
        + Neg<Output = T>
        + for<'b> Mul<&'b T, Output = T>
        + for<'b> BitXor<&'b T, Output = T>
        + Not<Output = T>
        + for<'b> Div<&'b T, Output = T>,
{
}
*/
