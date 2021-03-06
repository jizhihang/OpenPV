function [unavailable_flag, unavailable_info] = imageNetUnavailableFLCKR(original_info, unavailable_info)
  %% some imageNet images from FLCKR are returned as default GIF images
  %% with the message "this photo currently unavailable"
  %% could directly compare images but this might be time consuming
  %% assume filesize and other image info fields are collectively unique
  unavailable_flag = 0;

  if isempty(unavailable_info)
    unavailable_info = struct;
    unavailable_info.Filename = "/Users/gkenyon/Pictures/imageNet/this_photo_is_currently_unavailable.GIF";
    unavailable_info.FileSize =  2900;
    unavailable_info.Height =  375;
    unavailable_info.Width =  500;
    unavailable_info.BitDepth =  8;
    unavailable_info.Format = "GIF";
    unavailable_info.LongFormat = "CompuServe graphics interchange format";
    unavailable_info.XResolution = 0;
    unavailable_info.YResolution = 0;
    unavailable_info.TotalColors =  64;
    unavailable_info.TileName = [];
    unavailable_info.AnimationDelay = 0;
    unavailable_info.AnimationIterations =  1;
    unavailable_info.ByteOrder = "undefined";
    unavailable_info.Gamma = 0;
    unavailable_info.Matte = 0;
    unavailable_info.ModulusDepth =  8;
    unavailable_info.Quality =  75;
    unavailable_info.QuantizeColors =  256;
    unavailable_info.ResolutionUnits = "undefined";
    unavailable_info.ColorType = "indexed";
    unavailable_info.View = [];
    unavailable_info.FileModDate = "27-Jul-2011 09:57:53";
  endif

  if (original_info.FileSize ~= unavailable_info.FileSize) || ...
	(original_info.Height ~= unavailable_info.Height) || ...
	(original_info.Width ~= unavailable_info.Width) || ...
	(original_info.BitDepth ~= unavailable_info.BitDepth) || ...
	any(original_info.Format ~= unavailable_info.Format) || ...
	(original_info.XResolution ~= unavailable_info.XResolution) || ...
	(original_info.YResolution ~= unavailable_info.YResolution) || ...
	(original_info.TotalColors ~= unavailable_info.TotalColors) || ...
	(original_info.ModulusDepth ~= unavailable_info.ModulusDepth) || ...
	(original_info.Quality ~= unavailable_info.Quality) || ...
	(original_info.QuantizeColors ~= unavailable_info.QuantizeColors)
    return;
  else
    unavailable_flag = 1;
  endif

endfunction
	