#include "seam_carver.hpp"

// implement the rest of SeamCarver's functions here
// returns the instance's image_
const ImagePPM& SeamCarver::GetImage() const { return image_; }

// returns the image's height
int SeamCarver::GetHeight() const { return height_; }

// returns the image's width
int SeamCarver::GetWidth() const { return width_; }

// returns the energy of the pixel at row col in image_
int SeamCarver::GetEnergy(int row, int col) const {
  Pixel up_col;
  Pixel low_col;
  Pixel up_row;
  Pixel low_row;

  if (col + 1 >= width_) {
    up_col = image_.GetPixel(row, 0);
  } else {
    up_col = image_.GetPixel(row, col + 1);
  }
  if (col - 1 < 0) {
    low_col = image_.GetPixel(row, width_ - 1);
  } else {
    low_col = image_.GetPixel(row, col - 1);
  }
  if (row + 1 >= height_) {
    up_row = image_.GetPixel(0, col);
  } else {
    up_row = image_.GetPixel(row + 1, col);
  }
  if (row - 1 < 0) {
    low_row = image_.GetPixel(height_ - 1, col);
  } else {
    low_row = image_.GetPixel(row - 1, col);
  }

  int energy = findSumVal(up_col, low_col) + findSumVal(up_row, low_row);
  return energy;
}

int SeamCarver::findColorVal(std::string color, Pixel up, Pixel low) const {
  int val = 0;
  if (color == "red") {
    val = (low.GetRed() - up.GetRed());
  }
  if (color == "green") {
    val = (low.GetGreen() - up.GetGreen());
  }
  if (color == "blue") {
    val = (low.GetBlue() - up.GetBlue());
  }
  return val;
}

int SeamCarver::findSumVal(Pixel up, Pixel low) const {
  int r = findColorVal("red", up, low);
  int b = findColorVal("blue", up, low);
  int g = findColorVal("green", up, low);
  int sum = (r * r) + (b * b) + (g * g);
  return sum;
}

// returns the horizontal seam of image_ with the least amount of
// energy
//
// the ith int in the returned array corresponds to which row at
// col i is in the seam. example:
//
//    | x |   |
// ---+---+---+---
//  x |   | x |
// ---+---+---+---
//    |   |   | x
// returns {1, 0, 1, 2}
int* SeamCarver::GetHorizontalSeam() const {
  int* seam = new int[width_];
  int** values = new int*[height_];
  for (int row = 0; row < height_; ++row) {
    values[row] = new int[width_];
  }
  int row = findMinPathHorz(values);
  seam[0] = row;

  for (int col = 1; col < width_; ++col) {
    if (row == height_ - 1) {
      if (values[row - 1][col] < values[row][col]) {
        row = row - 1;
        seam[col] = row;
        continue;
      }
    }
    if (row == 0) {
      if (values[row + 1][col] < values[row][col]) {
        row = row + 1;
        seam[col] = row;
        continue;
      }
    }
    if (row > 0 && row < height_ - 1) {
      if (values[row - 1][col] < values[row][col] &&
          values[row - 1][col] <= values[row + 1][col]) {
        row = row - 1;
        seam[col] = row;
        continue;
      }
    }
    if (row < height_ - 1) {
      if (values[row + 1][col] < values[row][col]) {
        row = row + 1;
        seam[col] = row;
        continue;
      }
    }
    seam[col] = row;
  }
  for (int row = 0; row < height_; ++row) {
    delete[] values[row];
  }
  delete[] values;

  return seam;
}

// returns the vertical seam of image_ with the least amount of
// energy
//
// the ith int in the returned array corresponds to which col at
// row i is in the seam. example:
//
//    | x |   |
// ---+---+---+---
//    |   | x |
// ---+---+---+---
//    |   | x |
// returns {1, 2, 2}
int* SeamCarver::GetVerticalSeam() const {
  int* seam = new int[height_];
  int** values = new int*[height_];
  for (int row = 0; row < height_; ++row) {
    values[row] = new int[width_];
  }
  int col = findMinPathVert(values);
  seam[0] = col;
  for (int row = 1; row < height_; ++row) {
    if (col == 0) {
      if (values[row][col + 1] < values[row][col]) {
        col = col + 1;
        seam[row] = col;
        continue;
      }
    }
    if (col == width_ - 1) {
      if (values[row][col - 1] < values[row][col]) {
        col = col - 1;
        seam[row] = col;
        continue;
      }
    }
    if (col > 0 && col < width_ - 1) {
      if (values[row][col - 1] < values[row][col] &&
          values[row][col - 1] <= values[row][col + 1]) {
        col = col - 1;
        // std::cout << col << std::endl;
        seam[row] = col;
        continue;
      }
    }
    if (col < width_ - 1) {
      if (values[row][col + 1] < values[row][col]) {
        col = col + 1;
        // std::cout << col << std::endl;
        seam[row] = col;
        continue;
      }
    }
    // std::cout << col << std::endl;
    seam[row] = col;
  }

  for (int row = 0; row < height_; ++row) {
    delete[] values[row];
  }
  delete[] values;
  return seam;
}

// removes one horizontal seam in image_. example:
//
// image_ before:
//  0 | 1 | 2 | 3
// ---+---+---+---
//  4 | 5 | 6 | 7
// ---+---+---+---
//  8 | 9 | 10| 11
//
// seam to remove:
//    | x |   |
// ---+---+---+---
//  x |   | x |
// ---+---+---+---
//    |   |   | x
//
// image_ after:
//  0 | 5 | 2 | 3
// ---+---+---+---
//  8 | 9 | 10| 7
void SeamCarver::RemoveHorizontalSeam() {
  int* seam = GetHorizontalSeam();
  image_.RemoveHorizontalSeam(seam);
  height_ = image_.GetHeight();
  SetImage(image_);
  delete[] seam;
}

// removes one vertical seam in image_. example:
//
// image_ before:
//  0 | 1 | 2 | 3
// ---+---+---+---
//  4 | 5 | 6 | 7
// ---+---+---+---
//  8 | 9 | 10| 11
//
// seam to remove:
//    | x |   |
// ---+---+---+---
//    |   | x |
// ---+---+---+---
//    |   | x |
//
// image_ after:
//  0 | 2 | 3
// ---+---+---
//  4 | 5 | 7
// ---+---+---
//  8 | 9 | 11
void SeamCarver::RemoveVerticalSeam() {
  int* seam = GetVerticalSeam();
  image_.RemoveVerticalSeam(seam);
  width_ = image_.GetWidth();
  SetImage(image_);
  delete[] seam;
}

int SeamCarver::findMinPathVert(int** values) const {
  int best = 0;

  for (int col_idx = 0; col_idx < width_; ++col_idx) {
    values[height_ - 1][col_idx] = GetEnergy(height_ - 1, col_idx);
  }
  for (int row_idx = height_ - 2; row_idx >= 0; --row_idx) {
    for (int col_idx = 0; col_idx < width_; ++col_idx) {
      best = values[row_idx + 1][col_idx];
      if (col_idx > 0) {
        if (best > values[row_idx + 1][col_idx - 1]) {
          best = values[row_idx + 1][col_idx - 1];
        }
      }
      if (col_idx < width_ - 1) {
        if (best > values[row_idx + 1][col_idx + 1]) {
          best = values[row_idx + 1][col_idx + 1];
        }
      }
      values[row_idx][col_idx] = best + GetEnergy(row_idx, col_idx);
    }
  }
  best = values[0][0];
  int col = 0;
  for (int col_idx = 0; col_idx < width_; ++col_idx) {
    if (values[0][col_idx] < best) {
      best = values[0][col_idx];
      col = col_idx;
    }
  }
  return col;
}

int SeamCarver::findMinPathHorz(int** values) const {
  int best;
  for (int row_idx = 0; row_idx < height_; ++row_idx) {
    values[row_idx][width_ - 1] = GetEnergy(row_idx, width_ - 1);
  }

  for (int col_idx = width_ - 2; col_idx >= 0; --col_idx) {
    for (int row_idx = 0; row_idx < height_; ++row_idx) {
      best = values[row_idx][col_idx + 1];
      if (row_idx > 0) {
        if (best > values[row_idx - 1][col_idx + 1]) {
          best = values[row_idx - 1][col_idx + 1];
        }
      }
      if (row_idx < height_ - 1) {
        if (best > values[row_idx + 1][col_idx + 1]) {
          best = values[row_idx + 1][col_idx + 1];
        }
      }
      values[row_idx][col_idx] = best + GetEnergy(row_idx, col_idx);
    }
  }

  best = values[0][0];
  int row = 0;
  for (int row_idx = 0; row_idx < height_; ++row_idx) {
    if (values[row_idx][0] < best) {
      best = values[row_idx][0];
      row = row_idx;
    }
  }
  return row;
}

/**
 * Add any helper methods you may need
 */

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}
