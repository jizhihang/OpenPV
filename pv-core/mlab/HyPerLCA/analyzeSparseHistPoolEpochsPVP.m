function [Sparse_hdr, Sparse_hist_pool_array, Sparse_times_array] = ...
      analyzeSparseHistPoolEpochsPVP(Sparse_list, ...
				     output_dir, ...
				     Sparse_hist_rank_array, ...
				     load_Sparse_flag, ...
				     plot_Sparse_flag, ...
				     fraction_Sparse_frames_read, ...
				     min_Sparse_skip, ...
				     fraction_Sparse_progress, ...
				     Sparse_min_val_array, ...
				     Sparse_max_val_array, ...
				     Sparse_mean_val_array, ...
				     Sparse_std_val_array, ...
				     Sparse_median_val_array, ...
				     nx_GT, ny_GT, ...
				     num_Sparse_hist_pool_bins, ...
				     num_procs, ...
				     num_epochs)
  %% analyze sparse activity pvp file.  
  %% Sparse_hist_pool_array returns a histogram over activity (activity distribution) for each feature for an output grid of size nx_GT X ny_GT.  num_Sparse_hist_pool_bins gives the number of bins in the histogram, which range from Sparse_min_val to Sparse_max_val.  
  
  if ~exist("load_Sparse_flag") || isempty(load_Sparse_flag)
    load_Sparse_flag = false;  %% true -> load Sparse arrays from file
  endif
  if ~exist("plot_Sparse_flag") || isempty(plot_Sparse_flag)
    plot_Sparse_flag = false;
  endif
  if ~exist("min_Sparse_skip") || isempty(min_Sparse_skip)
    min_Sparse_skip = 0;  %% 0 -> skip no frames
  endif
  if ~exist("fraction_Sparse_frames_read") || isempty(fraction_Sparse_frames_read)
    fraction_Sparse_frames_read = 1;  %% 1 -> read all frames
  endif
  if ~exist("fraction_Sparse_progress") || isempty(fraction_Sparse_progress)
    fraction_Sparse_progress = 10;
  endif
  if ~exist("num_procs") || isempty(num_procs)
    num_procs = 1;
  endif
  if ~exist("num_epochs") || isempty(num_epochs)
    num_epochs = 1;
  endif
  
  Sparse_hist_pool_array = [];
  Sparse_times_array = [];
  
  num_Sparse_list = size(Sparse_list,1);
  if num_Sparse_list ==0
    warning(["analyzeSparseHistPoolEpochsPVP:num_Sparse_list == 0"]);
    return;
  endif
  Sparse_hdr = cell(num_Sparse_list,1);
  Sparse_hist_pool_array = cell(num_Sparse_list,1);
  Sparse_times_array = cell(num_Sparse_list,1);
  Sparse_struct_array = cell(num_Sparse_list,1);
  Sparse_dir = [output_dir, filesep, "Sparse"]
  [status, msg, msgid] = mkdir(Sparse_dir);
  if status ~= 1
    warning(["mkdir(", Sparse_dir, ")", " msg = ", msg]);
  endif 
  for i_Sparse = 1 : num_Sparse_list
    Sparse_file = [output_dir, filesep, Sparse_list{i_Sparse,1}, Sparse_list{i_Sparse,2}, ".pvp"]
    if ~exist(Sparse_file, "file")
      warning(["file does not exist: ", Sparse_file]);
      continue
    endif
    Sparse_fid = fopen(Sparse_file);
    Sparse_hdr{i_Sparse} = readpvpheader(Sparse_fid);
    fclose(Sparse_fid);
    tot_Sparse_frames = Sparse_hdr{i_Sparse}.nbands;
    num_Sparse_skip = tot_Sparse_frames - fix(tot_Sparse_frames/fraction_Sparse_frames_read);  %% number of activity frames to analyze, counting backward from last frame, maximum is tot_Sparse_frames
    num_Sparse_skip = max(num_Sparse_skip, min_Sparse_skip);
    progress_step = ceil(tot_Sparse_frames / fraction_Sparse_progress);

    %% analyze sparse (or non-sparse) activity from pvp file
    if ~load_Sparse_flag
      nx_Sparse = Sparse_hdr{i_Sparse}.nx;
      ny_Sparse = Sparse_hdr{i_Sparse}.ny;
      nf_Sparse = Sparse_hdr{i_Sparse}.nf;
      n_Sparse = nx_Sparse * ny_Sparse * nf_Sparse;
      Sparse_max_val = max(Sparse_max_val_array{i_Sparse}(:));
      Sparse_min_val = min(Sparse_min_val_array{i_Sparse}(:));;
      Sparse_mean_val = mean(Sparse_mean_val_array{i_Sparse}(:));
      Sparse_std_val = mean(Sparse_std_val_array{i_Sparse}(:));
      Sparse_median_val = median(Sparse_median_val_array{i_Sparse}(:));
      
      hist_bin_size_bottom = (Sparse_median_val - Sparse_min_val) / (num_Sparse_hist_pool_bins/2);
      hist_bin_size_top = (Sparse_max_val - Sparse_median_val) / (num_Sparse_hist_pool_bins/2);
      %%hist_bin_size = (Sparse_max_val - Sparse_min_val) / num_Sparse_hist_pool_bins;
      %%hist_edges = [Sparse_min_val : hist_bin_size : Sparse_max_val];
      %%Sparse_hist_pool_bins = [(Sparse_min_val + hist_bin_size/2) : hist_bin_size : (Sparse_max_val - hist_bin_size/2)];
      Sparse_hist_pool_bins_bottom = [(Sparse_min_val + hist_bin_size_bottom/2) : hist_bin_size_bottom : (Sparse_median_val - hist_bin_size_bottom/2)];
      Sparse_hist_pool_bins_top = [(Sparse_median_val + hist_bin_size_top/2) : hist_bin_size_top : (Sparse_max_val - hist_bin_size_top/2)];
      Sparse_hist_pool_bins = [Sparse_hist_pool_bins_bottom, Sparse_hist_pool_bins_top];
      
      nx_full_cell = cell(1);
      nx_full_cell{1} = nx_Sparse;
      ny_full_cell = cell(1);
      ny_full_cell{1} = ny_Sparse;
      nf_full_cell = cell(1);
      nf_full_cell{1} = nf_Sparse;
      nx_GT_cell = cell(1);
      nx_GT_cell{1} = nx_GT;
      ny_GT_cell = cell(1);
      ny_GT_cell{1} = ny_GT;
      Sparse_max_val_cell = cell(1);
      Sparse_max_val_cell{1} = Sparse_max_val;
      Sparse_min_val_cell = cell(1);
      Sparse_min_val_cell{1} = Sparse_min_val;
      Sparse_mean_val_cell = cell(1);
      Sparse_mean_val_cell{1} = Sparse_mean_val;
      Sparse_std_val_cell = cell(1);
      Sparse_std_val_cell{1} = Sparse_std_val;
      Sparse_median_val_cell = cell(1);
      Sparse_median_val_cell{1} = Sparse_median_val;
      num_Sparse_hist_pool_bins_cell = cell(1);
      num_Sparse_hist_pool_bins_cell{1} = num_Sparse_hist_pool_bins;
      

      if num_epochs == 1

	[Sparse_struct, Sparse_hdr_tmp] = ...
	    readpvpfile(Sparse_file, progress_step, tot_Sparse_frames, num_Sparse_skip,1);
	num_Sparse_frames = size(Sparse_struct,1);
      
	if num_procs == 1
	  [Sparse_times_list, ...
	   Sparse_hist_pool] = ...
	      cellfun(@calcSparseHistPVPArray, ...
		      Sparse_struct, ...
			 nx_full_cell, ...
			 ny_full_cell, ...
			 nf_full_cell, ...
			 nx_GT_cell, ...
			 ny_GT_cell, ...
			 Sparse_min_val_cell, ...
			 Sparse_max_val_cell, ...
			 Sparse_mean_val_cell, ...
			 Sparse_std_val_cell, ...
			 Sparse_median_val_cell, ...
			 num_Sparse_hist_pool_bins_cell, ...
		      "UniformOutput", false);
	elseif num_procs > 1
	  [Sparse_times_list, ...
	   Sparse_hist_pool] = ...
	      parcellfun(num_procs, ...
			 @calcSparseHistPVPArray, ...
			 Sparse_struct, ...
			 nx_full_cell, ...
			 ny_full_cell, ...
			 nf_full_cell, ...
			 nx_GT_cell, ...
			 ny_GT_cell, ...
			 Sparse_min_val_cell, ...
			 Sparse_max_val_cell, ...
			 Sparse_mean_val_cell, ...
			 Sparse_std_val_cell, ...
			 Sparse_median_val_cell, ...
			 num_Sparse_hist_pool_bins_cell, ...
			 "UniformOutput", false);
	endif  %% num_procs

 	Sparse_times = cell2mat(Sparse_times_list);
	num_times = length(Sparse_times);
	%%Sparse_hist_pool= zeros(num_Sparse_hist_pool_bins, nf_Sparse, ny_GT, nx_GT, num_times);
	%%if ~isempty(Sparse_hist_pool_list)
	%%  %%Sparse_hist_pool = cell2mat(Sparse_hist_pool_list);
	%%  %%keyboard
	%%  for i_time = 1 : num_times
	%%    Sparse_hist_pool(:,:,:,:,i_time) = Sparse_hist_pool_list{i_time};
	%%  endfor
	%%else
	%%  Sparse_hist_pool= zeros(num_Sparse_hist_pool_bins, nf_Sparse, ny_GT, nx_GT, num_times);
	%%endif

     else %% num_epochs

	num_frames_per_epoch = floor((tot_Sparse_frames-1)/num_epochs);
	for i_epoch = 1 : num_epochs

	  [Sparse_struct_epoch, Sparse_hdr_tmp] = ...
	      readpvpfile(Sparse_file, progress_step, i_epoch*num_frames_per_epoch, 1+(i_epoch-1)*num_frames_per_epoch,1);
	  num_Sparse_frames_epoch = size(Sparse_struct_epoch,1);

 	  if num_procs == 1
	    [Sparse_times_list_epoch, ...
	     Sparse_hist_pool_list_epoch] = ...
		cellfun(@calcSparseHistPVPArray, ...
			Sparse_struct_epoch, ...
			 nx_full_cell, ...
			 ny_full_cell, ...
			 nf_full_cell, ...
			 nx_GT_cell, ...
			 ny_GT_cell, ...
			 Sparse_min_val_cell, ...
			 Sparse_max_val_cell, ...
			 Sparse_mean_val_cell, ...
			 Sparse_std_val_cell, ...
			 Sparse_median_val_cell, ...
			 num_Sparse_hist_pool_bins_cell, ...
			"UniformOutput", false);
	  else
	    [Sparse_times_list_epoch, ...
	     Sparse_hist_pool_list_epoch] = ...
		parcellfun(num_procs, ...
			   @calcSparsePVPArray2, ...
			   Sparse_struct_epoch, ...
			   nx_full_cell, ...
			   ny_full_cell, ...
			   nf_full_cell, ...
			   nx_GT_cell, ...
			   ny_GT_cell, ...
			   Sparse_min_val_cell, ...
			   Sparse_max_val_cell, ...
			 Sparse_mean_val_cell, ...
			 Sparse_std_val_cell, ...
			 Sparse_median_val_cell, ...
			   num_Sparse_hist_pool_bins_cell, ...
			   "UniformOutput", false);
	  endif %% num_procs	
	  if i_epoch == 1
	    Sparse_times = cell2mat(Sparse_times_list_epoch);
	    if ~isempty(Sparse_hist_pool_list_epoch)
	      Sparse_hist_pool = cell2mat(Sparse_hist_pool_list_epoch); %%reshape(cell2mat(Sparse_hist_pool_list_epoch), [nf_Sparse, length(Sparse_times_list_epoch)]);
	    else
	      Sparse_hist_pool = zeros(num_Sparse_hist_pool_bins, nf_Sparse, ny_GT, nx_GT, length(Sparse_times));
	    endif
	  else
	    Sparse_times = [Sparse_times; cell2mat(Sparse_times_list_epoch)];
 	    if ~isempty(Sparse_hist_pool_list_epoch)
	      Sparse_hist_pool = [Sparse_hist_pool; cell2mat(Sparse_hist_pool_list_epoch)]; 
	    endif
	  endif %% i_epoch == 1
	endfor %% i_epoch
      endif %% num_epochs

      num_Sparse_frames = size(Sparse_times,1);      
      
      Sparse_filename_id = [Sparse_list{i_Sparse,2}, "_", "HistPool", "_", ...
			    num2str(Sparse_times(num_Sparse_frames), "%08d")];


      save("-mat", ...
	   [Sparse_dir, filesep, Sparse_filename_id, "_bins", ".mat"], ...
	   "Sparse_hist_pool_bins");
      save("-mat", ...
	   [Sparse_dir, filesep, Sparse_filename_id, ".mat"], ...
	   "Sparse_hist_pool");

    else  %% load Sparse data structures from file


      Sparse_hist_pool_bins_str = ...
	  [Sparse_dir, filesep, "Sparse_hist_pool_bins_", Sparse_list{i_Sparse,2}, "_", "*", ".mat"];
      Sparse_hist_pool_bins_glob = glob(Sparse_hist_pool_bins_str);
      num_Sparse_hist_pool_bins_glob = length(Sparse_hist_pool_bins_glob);
      if num_Sparse_hist_pool_bins_glob <= 0
	warning(["load_Sparse_flag is true but no files to load in: ", Sparse_hist_pool_bins_str]);
	break;
      endif
      load("-mat", Sparse_hist_pool_bins_glob{num_Sparse_hist_pool_bins_glob});
      Sparse_hist_pool_str = ...
	  [Sparse_dir, filesep, "Sparse_hist_pool_", Sparse_list{i_Sparse,2}, "_", "*", ".mat"];
      Sparse_hist_pool_glob = glob(Sparse_hist_pool_str);
      num_Sparse_hist_pool_glob = length(Sparse_hist_pool_glob);
      if num_Sparse_hist_pool_glob <= 0
	warning(["load_Sparse_flag is true but no files to load in: ", Sparse_hist_pool_str]);
	break;
      endif
      load("-mat", Sparse_hist_pool_glob{num_Sparse_hist_pool_glob});

      Sparse_filename_id = [Sparse_list{i_Sparse,2}, "_", ...
			    num2str(Sparse_times(end), "%08d")];

    endif %% load_Sparse_flag

    num_Sparse_frames = length(Sparse_times);
    if plot_Sparse_flag 
      if ~exist("Sparse_hist_rank_array") || isempty("Sparse_hist_rank_array")
	Sparse_rank = 1:nf_Sparse;
      else
	Sparse_rank = Sparse_hist_rank_array{i_Sparse};
	if isempty(Sparse_rank)
	  Sparse_rank = 1:nf_Sparse;
	endif
      endif
      Sparse_hist_pool_fig = figure;
      axis off; box off;
      k_subplot = 0;
      Sparse_hist_pool_hndl = zeros(ny_GT, nx_GT);
      Sparse_hist_pool_axis = zeros(ny_GT, nx_GT);
      for j_yGT = 1 : ny_GT
	for i_xGT = 1 : nx_GT
	  k_subplot = k_subplot + 1;
	  Sparse_hist_pool_axis(j_yGT,i_xGT) = subplot(ny_GT, nx_GT, k_subplot);
	  hist_pool_tmp = squeeze(Sparse_hist_pool{num_Sparse_frames}(:,:,j_yGT,i_xGT));
	  ranked_hist_pool = hist_pool_tmp(:, Sparse_rank);
	  bar(ranked_hist_pool(2:num_Sparse_hist_pool_bins,2:ceil(nf_Sparse/48):nf_Sparse)', 'stacked');
	  axis tight;
	  axis off;
	  box off;
	  colormap(prism)
	endfor
      endfor
      set(Sparse_hist_pool_fig, "name", [Sparse_filename_id]);
      saveas(Sparse_hist_pool_fig, ...
	     [Sparse_dir, filesep, Sparse_filename_id], "png");
    endif  %% plot_Sparse_flag 
    
    Sparse_hist_pool_array{i_Sparse} = Sparse_hist_pool;
    Sparse_times_array{i_Sparse} = Sparse_times;

    if num_Sparse_list > 10
      close(Sparse_hist_pool_fig);
    endif
    
  endfor  %% i_Sparse
  
endfunction
