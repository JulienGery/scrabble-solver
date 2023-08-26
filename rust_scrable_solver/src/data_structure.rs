use std::ops;

pub struct DataStructure {
    words: Vec<u8>,
    indexes: Vec<usize>,
}

//
//TODO add iter

impl ops::Index<usize> for DataStructure {
    type Output = [u8];

    fn index(&self, index: usize) -> &Self::Output {
        if index == 0 {
            let end = self.indexes[0];
            return &self.words[..end];
        }

        if index == self.indexes.len() - 1 {
            let start = self.indexes.last().unwrap();
            return &self.words[*start + 1..];
        }

        let start = self.indexes[index - 1];
        let end = self.indexes[index];
        return &self.words[start + 1..end];
    }
}

impl DataStructure {

    pub fn new(words: Vec<u8>, separator: u8) -> DataStructure {
        let mut indexes = vec![0];

        for i in 0..words.len() {
            if words[i] == separator {
                indexes.push(i);
            }
        }

        println!("indexes count: {}", indexes.len());
        return DataStructure {words, indexes};
    }

    pub fn len(&self) -> usize {
        return self.indexes.len();
    }
}
