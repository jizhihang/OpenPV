function ...
  [mass_xcorr, ...
   mass_autocorr, ...
   mass_xcorr_mean, ...
   mass_xcorr_std, ...
   xcorr_array, ...
   xcorr_dist ] = ...
      pvp_sparseXCorr(layer, ...
		      epoch_struct, ...
		      layer_struct, ...
		      target_struct, ...
		      xcorr_struct, ...
		      mass_xcorr, ...
		      mass_autocorr, ...
		      mass_xcorr_mean, ...
		      mass_xcorr_std, ...
		      xcorr_array, ...
		      xcorr_dist)


  global BIN_STEP_SIZE DELTA_T
  global pvp_order

  stim_steps = ...
      epoch_struct.stim_begin_step(layer) : epoch_struct.stim_end_step(layer);
  stim_bins = ...
      epoch_struct.stim_begin_bin(layer) : epoch_struct.stim_end_bin(layer);

%% init data structures
  for i_mode = 1 : xcorr_struct.num_modes
    mass_xcorr{i_mode,layer} = ...
	zeros( 2 * xcorr_struct.max_lag + 1, 1 );
    mass_autocorr{i_mode,layer} = ...
	zeros( 2 * xcorr_struct.max_lag + 1, 1 );
    xcorr_array{i_mode,layer} = ...
        zeros( xcorr_struct.num_power_mask(layer, i_mode) );
  endfor %%  %i_mode

  %% start loop over epochs
  for i_epoch = 1 : epoch_struct.num_epochs
    disp(['i_epoch = ', num2str(i_epoch)]);
    
    %% read spike train for this epoch
    [spike_array] = ...
        pvp_readSparseSpikes(layer, ...
			     i_epoch, ...
			     epoch_struct, ...
			     layer_struct, ...
			     pvp_order);
    if isempty(spike_array)
      continue;
    endif %%

    %% compute/accumulate xcorr
    %% extract scalar pairwise correlations
    for i_mode = 1 : xcorr_struct.num_modes  % 1 = peak, 2 = mean
      
      disp( ['computing xcorr', ...
             '(', ...
               num2str(layer), ...
               ',', ...
               num2str(i_mode), ...
               ')'] );

      xcorr_flag = 1;
      is_auto = 1;
      [mass_xcorr_tmp, ...
       mass_autocorr_tmp, ...
       mass_xcorr_mean_tmp, ...
       mass_xcorr_std_tmp, ...
       xcorr_array_tmp, ...
       xcorr_dist] = ...
          pvp_xcorr2(spike_array(stim_steps, ...
				 xcorr_struct.power_mask{layer, i_mode}), ...
                     spike_array(stim_steps, ...
				 xcorr_struct.power_mask{layer, i_mode}), ...
                     xcorr_struct, ...
                     xcorr_struct.power_mask{layer, i_mode}, ...
                     layer_struct.size_layer{layer}, ...
                     xcorr_struct.power_mask{layer, i_mode}, ...
                     layer_struct.size_layer{layer}, ...
                     is_auto, ...
                     xcorr_flag);
      mass_xcorr{i_mode, layer} =  mass_xcorr{i_mode, layer} + ...
          mass_xcorr_tmp;
      mass_autocorr{i_mode, layer} =  mass_autocorr{i_mode, layer} + ...
          mass_autocorr_tmp;
      xcorr_array{i_mode, layer} = xcorr_array{i_mode, layer} + ...
          xcorr_array_tmp(:, :, i_mode);
      
    endfor %% % i_mode
        
  endfor %% % i_epoch
  
  
