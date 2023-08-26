use std::ops;

pub struct Board<T> {
    pub board: Vec<T>,
    pub width: usize,
    pub height: usize,
}

impl<T> ops::Index<usize> for Board<T> {
    type Output = [T];

    fn index(&self, index: usize) -> &Self::Output {
        return &self.board[index * self.width..(index + 1) * self.width];
    }
}

impl<T> ops::IndexMut<usize> for Board<T> {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        return &mut self.board[index * self.width..(index + 1) * self.width];
    }
}

impl<T: std::fmt::Display> std::fmt::Display for Board<T> {

    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result{
        let mut ret = String::with_capacity(self.width * self.height * 3);

        for j in 0..self.height {
            for i in 0..self.width {
                ret += &format!("{} ", self[j][i]);
            }
            ret.push('\n');
        }

        return write!(f, "{}", ret);
    }
}

impl<T: std::clone::Clone + std::fmt::Display> Board<T> {
    pub fn new(value: T, width: usize, height: usize) -> Board<T> {
        let board : Vec<T> = vec![value; width * height];
        return Board { board, width, height };
    }

    pub fn rotate_board(&self) -> Board<T> {
        let mut ret = Board::new(self.board[0].clone(), self.height, self.width);

        for j in 0..self.height {
            for i in 0..self.width {
                ret[j][i] = self[i][j].clone();
            }
        }
        return ret;
    }
}


