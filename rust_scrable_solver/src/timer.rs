use std::time;

pub struct Timer<'a>
{
    start: time::Instant,
    message: &'a str,
}

impl<'a> Timer<'a> {
    pub fn new(message: &'a str) -> Timer {
        return Timer{start: time::Instant::now(), message};
    }
}


impl<'a> Drop for Timer<'a> {

    fn drop(&mut self) {
     let end = time::Instant::now();
     let delta = end - self.start;

     println!("{}: {}ms", self.message, delta.as_millis());
    }
}
