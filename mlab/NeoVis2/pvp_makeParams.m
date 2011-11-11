function [pvp_params_file, ...
	  pvp_params_dir] = ...
      pvp_makeParams(NEOVISION_DATASET_ID, ...
		     NEOVISION_DISTRIBUTION_ID, ...
		     pvp_repo_path, ...
		     pvp_program_path, ...
		     pvp_params_template, ...
		     pvp_clip_name, ...
		     pvp_object_type, ...
		     pvp_num_ODD_kernels, ...
		     pvp_edge_type, ...
		     pvp_frame_size, ...
		     pvp_fileOfFrames)
  
  global PVP_VERBOSE_FLAG
  if ~exist("PVP_VERBOSE_FLAG") || isempty(PVP_VERBOSE_FLAG)
    PVP_VERBOSE_FLAG = 0;
  endif
  global pvp_home_path
  global pvp_mlab_path
  if isempty(pvp_home_path)
    pvp_home_path = ...
	[filesep, "home", filesep, "garkenyon", filesep];
  endif
  if isempty(pvp_mlab_path)
    pvp_mlab_path = ...
	[pvp_home_path, "workspace-indigo", filesep, "PetaVision", filesep, "mlab", filesep];
  endif

  more off;
  begin_time = time();

  num_input_args = 0
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("NEOVISION_DATASET_ID") || isempty(NEOVISION_DATASET_ID)
    NEOVISION_DATASET_ID = "Heli"; %% "Tower"; %% "Tail"; %% 
  endif
  neovision_dataset_id = tolower(NEOVISION_DATASET_ID); %% 
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("NEOVISION_DISTRIBUTION_ID") || isempty(NEOVISION_DISTRIBUTION_ID)
    NEOVISION_DISTRIBUTION_ID = "Formative"; %% "Training"; %%  "Challenge"; %%
  endif
  neovision_distribution_id = tolower(NEOVISION_DISTRIBUTION_ID); %% 
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_repo_path") || isempty(pvp_repo_path)
    pvp_repo_path = [filesep, "mnt", filesep, "data1", filesep, "repo", filesep];
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_program_path") || isempty(pvp_program_path)
    pvp_program_path = [pvp_repo_path, "neovision-programs-petavision", filesep];
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_params_template") || isempty(pvp_params_template)
    pvp_params_template = ...
	[pvp_program_path, ...
	 NEOVISION_DATASET_ID, filesep, ...
	 NEOVISION_DISTRIBUTION_ID, filesep, ...
	 "params", filesep, "pvp_template.params"];
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_clip_name") || isempty(pvp_clip_name)
    pvp_clip_name =  "050"; %%
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_object_type") || isempty(pvp_object_type)
    pvp_object_type =  "Car"; %%
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_num_ODD_kernels") || isempty(pvp_num_ODD_kernels)
    pvp_num_ODD_kernels =  3; %%
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_edge_type") || isempty(pvp_edge_type)
    pvp_edge_type =  "DoG"; %%
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_frame_size") || isempty(pvp_frame_size)
    pvp_frame_size =  [1080 1920]; %%
  endif
  num_input_args = num_input_args + 1;
  if nargin < num_input_args || ~exist("pvp_fileOfFrames") || isempty(pvp_fileOfFrames)
    pvp_fileOfFrames_path = ...
	[list_path, NEOVISION_DATASET_ID, filesep, NEOVISION_DISTRIBUTION_ID, filesep, ...
	 pvp_clip_name, filesep, pvp_edge_type, filesep];
    pvp_fileOfFrames_prefix = ...
	[NEOVISION_DATASET_ID, "_", NEOVISION_DISTRIBUTION_ID, "_", pvp_clip_name, "_", pvp_edge_type, "_", "frames"];
    pvp_fileOfFrames = ...
	[pvp_fileOfFrames_path, pvp_fileOfFrames_prefix, ".txt"];
  endif

  pvp_params_dir = ...
      [pvp_program_path, ...
       NEOVISION_DATASET_ID, filesep, ...
       NEOVISION_DISTRIBUTION_ID, filesep, ...
       "params", filesep, ...
       pvp_clip_name, filesep, ...
       pvp_object_type, filesep, ...
       pvp_edge_type, filesep];
  pvp_params_file = ...
      [pvp_params_dir, ...
       NEOVISION_DATASET_ID, ...
       "_", ...
       pvp_object_type, ...
       num2str(pvp_num_ODD_kernels), ...
       "_", ...
       pvp_edge_type, ...
       ".params"];
  pvp_output_path = ...
      [NEOVISION_DATASET_ID, filesep, NEOVISION_DISTRIBUTION_ID, filesep, ...
       "activity", filesep,  ...
       pvp_clip_name, filesep, ...
       pvp_object_type, filesep, ...
       pvp_edge_type, filesep];
  pvp_weights_path = ...
      [NEOVISION_DATASET_ID, filesep, NEOVISION_DISTRIBUTION_ID, filesep, ...
       "weights", filesep,  ...
       pvp_clip_name, filesep, ...
       pvp_object_type, filesep, pvp_edge_type, filesep];
  pvp_weights_L1ToL1 = [pvp_weights_path, "a4_last.pvp"];
  pvp_weights_L2ToL2 = [pvp_weights_path, "a6_last.pvp"];
  pvp_weights_L3ToL3 = [pvp_weights_path, "a8_last.pvp"];
  pvp_weights_L4ToL4 = [pvp_weights_path, "a8_last.pvp"];
      

  pvp_params_fid = ...
      fopen(pvp_params_file, "w");
  pvp_template_fid = ...
      fopen(pvp_params_template, "r");

  pvp_params_token = "$$$";
  pvp_params_hash = ...
      {"nx", "nx", pvp_frame_size(2); ...
       "ny", "ny", pvp_frame_size(1); ...
       "numSteps", "numSteps", pvp_num_frames + pvp_num_ODD_kernels + 3; ...
       "outputPath", "outputPath", pvp_fileOfFrames; ...
       "imageListPath", "imageListPath", pvp_fileOfFrames; ...
       "burstDuration", "burstDuration", pvp_num_frames; ...
       "endStim", "endStim", pvp_num_frames; ...
       "VgainL1Clique", "Vgain", 0.25; ...
       "VgainL2Clique", "Vgain", 1.0; ...
       "VgainL3Clique", "Vgain", 1.0; ...
       "VgainL4Clique", "Vgain", 1.0; ...
       "L1CliqueToL1Clique", "InitWeightsFile", pvp_weights_L1ToL1; ...
       "L2CliqueToL2Clique", "InitWeightsFile", pvp_weights_L2ToL2; ...
       "L3CliqueToL3Clique", "InitWeightsFile", pvp_weights_L3ToL3; ...
       "L4CliqueToL4Clique", "InitWeightsFile", pvp_weights_L4ToL4; ...
       };
  pvp_num_params = size(pvp_params_hash, 1);
       

  while(~feof(pvp_template_fid))
    pvp_template_str = gets(pvp_template_fid);
    for pvp_params_ndx = 1 : pvp_num_params
      pvp_str_ndx = ...
	  strfind(pvp_template_str, ...
		  [pvp_params_token, ...
		   pvp_params_hash{pvp_params_ndx, 1}, ...
		   pvp_params_token])
      if ~isempty(pvp_str_ndx)
	pvp_hash_len_ = ...
	    length(pvp_params_hash{pvp_params_ndx, 1});
	pvp_template_len = ...
	    length(pvp_params_hash{pvp_params_ndx, 1});
	pvp_prefix = pvp_template_str(1:pvp_str_ndx-1);
	pvp_suffix = pvp_template_str(pvp_str_ndx+pvp_hash_len-1:pvp_template_len);
	pvp_params_str = ...
	    [pvp_prefix, ...
	     pvp_params_hash{pvp_params_ndx, 2}, ...
	     " = ", ...
	     num2str(pvp_params_hash{pvp_params_ndx, 3}), ...
	     pvp_suffix];
      else
	pvp_params_str = pvp_template_str;
      endif
      fputs(pvp_params_str);
    endfor
  endwhile
  fclose(pvp_params_fid);
  fclose(pvp_template_fid);

endfunction %% pvp_makePetaVisionParamsFile
