#pragma once
#include <string>
#include <unordered_map>

enum class Operator
{
    Equals,
    NotEquals,
    EqualsStrict,
    NotEqualsStrict,
    Assign,
    Less,
    More,
    LessEq,
    MoreEq,
    Add,
    Sub,
    Mul,
    Div,
    Negate,
    AddAssign,
    SubAssign,
    MulAssign,
    DivAssign,
    ModuloAssign,
    Modulo,
    And,
    Or,
    Not,
    BitAnd,
    BitOr,
    BitXor,
    BitNot,
    BitLeftShift,
    BitRightShift,
    BitAndAssign,
    BitOrAssign,
    BitXorAssign,
    BitNotAssign,
    BitLeftShiftAssign,
    BitRightShiftAssign,
};

static const std::unordered_map<std::string, Operator> Operators = {
    {"===", Operator::EqualsStrict},
    {"==", Operator::Equals},
    {"!==", Operator::NotEqualsStrict},
    {"!=", Operator::NotEquals},
    {">=", Operator::MoreEq},
    {"<=", Operator::LessEq},
    {">>", Operator::BitRightShift},
    {"<", Operator::Less},
    {">", Operator::More},
    {"+", Operator::Add},
    {"-", Operator::Sub},
    {"*", Operator::Mul},
    {"/", Operator::Div},
    {"&&", Operator::And},
    {"||", Operator::Or},
    {"and", Operator::And},
    {"or", Operator::Or},
    {"%", Operator::Modulo},
};

static const std::unordered_map<std::string, Operator> UnaryOperators = {
    {"!", Operator::Not},
    {"not", Operator::Not},
    {"neg", Operator::Negate},
    {"~", Operator::BitNot},
};

static const std::unordered_map<std::string, Operator> AssignOperators = {

    {"=", Operator::Assign},
    {">>=", Operator::BitRightShiftAssign},
    {"<<=", Operator::BitLeftShiftAssign},
    {">>", Operator::BitRightShift},

    {"+=", Operator::AddAssign},
    {"-=", Operator::SubAssign},
    {"*=", Operator::MulAssign},
    {"/=", Operator::DivAssign},

    {"&=", Operator::BitOrAssign},
    {"&=", Operator::BitAndAssign},
    {"~=", Operator::BitNotAssign},
    {"^=", Operator::BitXorAssign},
    {"%=", Operator::ModuloAssign},
};