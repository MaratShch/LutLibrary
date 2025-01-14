import os
import struct

def concatenate_idat_chunks(png_data):
    """
    Concatenates the data from all IDAT chunks in a PNG file into a single byte array,
    also prints where each IDAT chunk data was added to the output.

    Args:
        png_data (bytes): The raw byte data of the PNG file.

    Returns:
        tuple (bytes, dict) : A single byte array containing the concatenated IDAT data (without chunk headers)
               or None if an error is encountered or no IDAT chunks were found, and a dictionary with debug information
    """
    idat_signature = b"IDAT"
    concatenated_data = bytearray()
    index = 8 # Skip the first 8 bytes of the file header
    idat_index = 0
    idat_chunks_found = False
    debug_info = {}

    while index < len(png_data):
        # Read chunk length
        length_bytes = png_data[index:index+4]
        if len(length_bytes) < 4:
             break;
        length = struct.unpack(">I", length_bytes)[0]
        index += 4

        # Read Chunk Type
        chunk_type = png_data[index:index+4]
        if len(chunk_type) < 4:
           break
        index += 4
        
        # Process IDAT chunks
        if chunk_type == idat_signature:
            idat_chunks_found = True
            data_start = index
            data_end = index + length
           
            if data_end > len(png_data):
              break;

            start_offset = len(concatenated_data)
            concatenated_data.extend(png_data[data_start : data_end])
            end_offset = len(concatenated_data)
            debug_info[idat_index] = {"start":start_offset, "end":end_offset}

            print(f"IDAT section {idat_index} adds {length} bytes by offset {start_offset} bytes")
            # Move to next chunk
            index += length
            index += 4  # Skip CRC
            idat_index +=1
        else:
            # Move to next chunk if this is not an IDAT section
            index += length
            index += 4  # Skip CRC
    if not idat_chunks_found:
        return None, None # No IDAT chunks found
    return bytes(concatenated_data), debug_info

def save_concatenated_data(concatenated_data, output_file, debug_info):
  """
  Saves the concatenated data to a human readable text file

  Args:
    concatenated_data (bytes): The concatenated data
    output_file (str) : name of the output file
    debug_info (dict): A dictionary containing debug information for each IDAT section

  """
  folder_path = "."
  full_file_path = os.path.join(folder_path, output_file)
  rowCounter = 32

  with open(full_file_path, 'w') as file:
    # Write the first line as a C-style comment with the data size
      file.write(f"/* DATA SIZE = {len(concatenated_data)} BYTES */\n")

      line_buffer = []
      current_offset = 0
      for idat_index in sorted(debug_info.keys()):
          data_start = debug_info[idat_index]["start"]
          data_end = debug_info[idat_index]["end"]
          file.write(f"/* this data concatenated from IDAT section number {idat_index}. Byte offset = {data_start} bytes */\n")
          for i in range(data_start, data_end):
               # Format the value as 0x-prefixed hexadecimal with leading zero
               line_buffer.append(f"0x{concatenated_data[i]:02X}, ")
               current_offset += 1

              # Every 32 values, write the line and reset the buffer
               if (current_offset) % rowCounter == 0:
                  file.write(''.join(line_buffer) + '\n')
                  line_buffer = []
      if line_buffer:
          file.write(''.join(line_buffer) + '\n')
      file.flush()
      os.fsync(file.fileno())


if __name__ == '__main__':
    # Example usage
    # Replace this with actual PNG file reading
    try:
        with open("AnalogFX-Sepia-Color.png", "rb") as file:
            png_data = file.read()
    except FileNotFoundError:
        print ("Can't find test.png file")
        exit()

    concatenated_idat_data, debug_info = concatenate_idat_chunks(png_data)

    if concatenated_idat_data:
        print(f"Total concatenated IDAT data length: {len(concatenated_idat_data)} bytes")
        save_concatenated_data(concatenated_idat_data, "concatenated_idat_data.txt", debug_info)
        # You can now process the concatenated_idat_data with DEFLATE decoder
    else:
        print ("No IDAT chunks found")

