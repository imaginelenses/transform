# Transform
A basic command-line image processing script for [bitmap](https://en.wikipedia.org/wiki/BMP_file_format) images written from scratch in C.

## Support
* Supports `BMP v3`, `BMP v4` and `BMP v5`.
* Supports `24-bit` and `32-bit` bitmap images.
* Does not support color palettes.
* Does not support compression.

## Usage 
```bash
./transform [options] <input file> <output file>
```
### Options:
* `-i` 

  Invert colors of image.
  ```bash
  ./transform sampleImages/marbles.bmp invert.bmp -i
  ```
  | Original | Inverted |
  |:--------:|:--------:|
  | ![Original](sampleImages/marbles.bmp) | ![Inverted](assets/invert.bmp) |
* `-l <factor>`

  Apply log gamma transform (natural log) with a multiplication `<factor>`.
  ```bash
  ./transform sampleImages/lightLarge.bmp log.bmp -l 30
  ```
  | Original | Log Gamma |
  |:--------:|:--------:|
  | ![Original](sampleImages/lightLarge.bmp) | ![Log Gamme](assets/log.bmp) |
* `-e <factor>`

  Apply inverse log gamma transform (natural log) with a multiplication `<factor>`.
  ```bash
  ./transform sampleImages/lightLarge.bmp inverseLog.bmp -e 0.001
  ```
  | Original | Inverse Log Gamma |
  |:--------:|:--------:|
  | ![Original](sampleImages/lightLarge.bmp) | ![Inverse Log Gamme](assets/inverseLog.bmp) |
* `-p <gamma>`

  Apply power-law gamma transform - RGB intensities are raised to the power `(1 / <gamma>)`.
  ```bash
  ./transform sampleImages/harvard.bmp gamma.bmp -p 2.0 
  ```
  | Original | Power-Law Gamma |
  |:--------:|:--------:|
  | ![Original](sampleImages/harvard.bmp) | ![Power-Law Gamme](assets/gamma.bmp) |
  
  To apply the inverse gamma transform use `(1 / original gamma)` as `<gamma>` value.
  
  **Example:** To get inverse of `<gamma> = 2.0` set `<gamma> = 0.5`
  ```bash
  ./transform sampleImages/harvard.bmp inverseGamma.bmp -p 0.5
  ```
  | Original | Inverse Power-Law Gamma |
  |:--------:|:--------:|
  | ![Original](sampleImages/harvard.bmp) | ![Inverse Power-Law Gamme](assets/inverseGamma.bmp) |
* `-g`
  
  Convert image to `8-bit` [greyscale image](https://en.wikipedia.org/wiki/Grayscale).
  
  **Note:** Any options provided after this will be discarded.
  ```bash
  ./transform sampleImages/cat.bmp grey.bmp -g
  ```
  | Original | Greyscale |
  |:--------:|:--------:|
  | ![Original](sampleImages/cat.bmp) | ![Power-Law Gamme](assets/grey.bmp) |
* `-b`
  
  Bit plane slicing:
  
  Convert image to greyscale and generate 8 `1-bit` [monochrome images](https://en.wikipedia.org/wiki/Monochrome) representing each [bit plane](https://en.wikipedia.org/wiki/Bit_plane) ranging from MSB (Most Significant Bit) to LSB (Least Significant Bit) - [Bit numbering](https://en.wikipedia.org/wiki/Bit_numbering).
  
  **Note:** Any options provided after this will be discarded.
  ```bash
  ./transform sampleImages/lightLarge.bmp out.bmp -b
  ```
  | Original | Greyscale |
  |:--------:|:--------:|
  | ![Original](sampleImages/lightLarge.bmp) | ![Greyscale](assets/bitPlaneSlicing/out.bmp) |
  
  | BitPlane 0 | BitPlane 1 | BitPlane 2 | BitPlane 3 | BitPlane 4 | BitPlane 5 | BitPlane 6 | BitPlane 7 |
  |:----------:|:----------:|:----------:|:----------:|:----------:|:----------:|:----------:|:----------:|
  | ![0](assets/bitPlaneSlicing/bitPlane0_out.bmp) | ![1](assets/bitPlaneSlicing/bitPlane1_out.bmp) | ![2](assets/bitPlaneSlicing/bitPlane2_out.bmp) | ![3](assets/bitPlaneSlicing/bitPlane3_out.bmp) | ![4](assets/bitPlaneSlicing/bitPlane4_out.bmp) | ![5](assets/bitPlaneSlicing/bitPlane5_out.bmp) | ![6](assets/bitPlaneSlicing/bitPlane6_out.bmp) | ![7](assets/bitPlaneSlicing/bitPlane7_out.bmp) |
  
  Set a range of RGB intensities to specified intensity.
  
  **Attributes:** `"Ra, Rb, Ri, Ga, Gb, Gi, Ba, Bb, Bi"`
  
  Intensities `a` to `b` are set to intensity `i`.
  ```bash
  ./transform sampleImages/harvard.bmp intensities.out -s150,255,255,50,100,100,200,255,200
  ```
#### Note:
The order in which the options are provided are the order in which the transforms are applied.
