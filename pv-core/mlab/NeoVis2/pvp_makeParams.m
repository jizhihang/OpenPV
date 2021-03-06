function [pvp_params_file, pvp_params_path] = ...
      pvp_makeParams(NEOVISION_DATASET_ID, ...
		     NEOVISION_DISTRIBUTION_ID, ...
		     pvp_repo_path, ...
		     pvp_program_path, ...
		     pvp_input_path, ...
		     pvp_clip_name, ...
		     pvp_object_type, ...
		     pvp_num_ODD_kernels, ...
		     pvp_bootstrap_str, ...
		     pvp_edge_type, ...
		     pvp_version_str, ...
		     pvp_params_template, ...
		     pvp_frame_size, ...
		     pvp_num_frames, ...
		     pvp_list_path, ...
		     pvp_fileOfFrames)
  
  %%keyboard;
  global PVP_VERBOSE_FLAG
  if ~exist("PVP_VERBOSE_FLAG") || isempty(PVP_VERBOSE_FLAG)
    PVP_VERBOSE_FLAG = 0;
  endif
  global pvp_home_path
  global pvp_workspace_path
  global pvp_mlab_path
  global pvp_clique_path
  if isempty(pvp_home_path)
    pvp_home_path = ...
	[filesep, "home", filesep, "garkenyon", filesep];
    %%[filesep, "Users", filesep, "gkenyon", filesep];
  endif
  if isempty(pvp_workspace_path)
    pvp_workspace_path = ...
	[pvp_home_path, "workspace-indigo", filesep];
  endif
  if isempty(pvp_mlab_path)
    pvp_mlab_path = ...
	[pvp_home_path, "workspace-indigo", filesep, "PetaVision", filesep, "mlab", filesep];
  endif
  if isempty(pvp_clique_path)
    pvp_clique_path = ...
	[pvp_workspace_path, "Clique2", filesep];
  endif

  more off;
  begin_time = time();

  num_argin = 0;
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("NEOVISION_DATASET_ID") || isempty(NEOVISION_DATASET_ID)
    NEOVISION_DATASET_ID = "Heli"; %% "Tower"; %% "Tailwind"; %% 
  endif
  neovision_dataset_id = tolower(NEOVISION_DATASET_ID); %% 
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("NEOVISION_DISTRIBUTION_ID") || isempty(NEOVISION_DISTRIBUTION_ID)
    NEOVISION_DISTRIBUTION_ID = "Training"; %% "Challenge"; %% "Formative"; %%  
  endif
  neovision_distribution_id = tolower(NEOVISION_DISTRIBUTION_ID); %% 
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_repo_path") || isempty(pvp_repo_path)
    pvp_repo_path = ...
	[filesep, "mnt", filesep, "data1", filesep, "repo", filesep];
	%%[pvp_home_path, "NeoVision2", filesep];
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_program_path") || isempty(pvp_program_path)
    pvp_program_path = ...
	[pvp_repo_path, "neovision-programs-petavision", filesep, ...
	 NEOVISION_DATASET_ID, filesep, ...
	 NEOVISION_DISTRIBUTION_ID, filesep];
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_input_path") || isempty(pvp_input_path)
    pvp_input_path = ...
	[pvp_clique_path, "input", filesep, ...
	 NEOVISION_DATASET_ID, filesep, ...
	 NEOVISION_DISTRIBUTION_ID, filesep];
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_clip_name") || isempty(pvp_clip_name)
    pvp_clip_name =  "045"; %%
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_object_type") || isempty(pvp_object_type)
    pvp_object_type =  "Car"; %%
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_num_ODD_kernels") || isempty(pvp_num_ODD_kernels)
    pvp_num_ODD_kernels =  3; %%
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_bootstrap_str") %% string can be empty
    pvp_bootstrap_str =  ""; %% "_bootstrap"; %%
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_edge_type") || isempty(pvp_edge_type)
    pvp_edge_type =  "canny"; %%
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_version_str") %% string can be empty
    pvp_version_str =  ""; %%
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_params_template") || isempty(pvp_params_template)
    pvp_params_template = ...
	[pvp_clique_path, ...
	 "templates", filesep, ...
	 NEOVISION_DATASET_ID, "_", NEOVISION_DISTRIBUTION_ID, "_", ...
	 pvp_object_type, num2str(pvp_num_ODD_kernels), pvp_bootstrap_str, "_", pvp_edge_type, pvp_version_str,  "_", ...
	 "template.params"];
  endif
  clip_log_dir = [pvp_program_path, ...
		  "log", filesep, pvp_clip_name, filesep];
  clip_log_pathname = [clip_log_dir, "log.txt"];
  clip_log_struct = struct;
  if exist(clip_log_pathname, "file")
    clip_log_fid = fopen(clip_log_pathname, "r");
    clip_log_struct.tot_unread = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_rejected = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_clips = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_DoG = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_canny = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_cropped = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_mean = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_std = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_border_artifact_top = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_border_artifact_bottom = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_border_artifact_left = str2num(fgets(clip_log_fid));
    clip_log_struct.tot_border_artifact_right = str2num(fgets(clip_log_fid));
    clip_log_struct.ave_original_size = str2num(fgets(clip_log_fid));
    clip_log_struct.ave_cropped_size = str2num(fgets(clip_log_fid));
    clip_log_struct.std_original_size = str2num(fgets(clip_log_fid));
    clip_log_struct.std_cropped_size = str2num(fgets(clip_log_fid));
    fclose(clip_log_fid);
  else
    clip_log_struct.tot_clips = 450;
    clip_log_struct.ave_cropped_size = [1080 1920];
  endif %% exist(clip_log_pathname)
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_frame_size") || isempty(pvp_frame_size)
    pvp_frame_size =  clip_log_struct.ave_cropped_size; %%
    disp(["frame_size = ", num2str(pvp_frame_size)]);
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_num_frames") || isempty(pvp_num_frames)
    pvp_num_frames =  clip_log_struct.tot_clips; %%
    disp(["num_frames = ", num2str(pvp_num_frames)]);
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_list_path") || isempty(pvp_list_path)
    pvp_list_path = ...
	[pvp_program_path, 
	 "list_", pvp_edge_type, filesep];
  endif
  num_argin = num_argin + 1;
  if nargin < num_argin || ~exist("pvp_fileOfFrames") || isempty(pvp_fileOfFrames)
    pvp_fileOfFrames_path = ...
	[list_path, pvp_clip_name, filesep];
    pvp_fileOfFrames_file = ...
	["train_fileOfFilenames.txt"];
	%%[NEOVISION_DATASET_ID, "_", NEOVISION_DISTRIBUTION_ID, "_", pvp_clip_name, "_", pvp_edge_type, "_", "frames"];
    pvp_fileOfFrames = ...
	[pvp_fileOfFrames_path, pvp_fileOfFrames_file];
  endif

  pvp_params_path = ...
      [pvp_input_path, ...
       pvp_clip_name, filesep, ...
       pvp_object_type, ...
       num2str(pvp_num_ODD_kernels), pvp_bootstrap_str, filesep, ...
       pvp_edge_type, pvp_version_str, filesep];
  pvp_params_file = ...
      [pvp_params_path, ...
       NEOVISION_DATASET_ID, ...
       "_", ...
       NEOVISION_DISTRIBUTION_ID, ...
       "_", ...
       pvp_clip_name, ...
       "_", ...
       pvp_object_type, ...
       num2str(pvp_num_ODD_kernels), pvp_bootstrap_str, ...
       "_", ...
       pvp_edge_type, ...
       pvp_version_str, ...
       ".params"];
  pvp_activity_path = ...
      [pvp_program_path, ...
       "activity", filesep, ...
       pvp_clip_name, filesep, ...
       pvp_object_type, ...
       num2str(pvp_num_ODD_kernels), pvp_bootstrap_str, filesep, ...
       pvp_edge_type, pvp_version_str, filesep];
       

 
  pvp_params_fid = ...
      fopen(pvp_params_file, "w");
  if pvp_params_fid < 0
    disp(["fopen failed: ", pvp_params_file]);
  end
  
  pvp_template_fid = ...
      fopen(pvp_params_template, "r");
  if pvp_template_fid < 0
    disp(["fopen failed: ", pvp_params_template]);
  end

  pvp_params_token_left = "$$$_";
  pvp_params_token_right = "_$$$";
  pvp_params_hash = ...
      {"nx", "nx",num2str(pvp_frame_size(2)); ...
       "ny", "ny", num2str(pvp_frame_size(1)); ...
       "numSteps", "numSteps", num2str(pvp_num_frames + pvp_num_ODD_kernels + 4); ...
       "outputPath", "outputPath", ["""", pvp_activity_path, """"]; ...
       "imageListPath", "imageListPath", ["""", pvp_fileOfFrames, """"]; ...
       "burstDuration", "burstDuration", num2str(pvp_num_frames); ...
       "endStim", "endStim", num2str(pvp_num_frames); ...
       "VgainL1Clique", "Vgain", num2str(0.03125); ...
       "VgainL2Clique", "Vgain", num2str(0.0625); ...
       "VgainL3Clique", "Vgain", num2str(0.0625); ...
       "VgainL4Clique", "Vgain", num2str(0.0625); ...
       };
  pvp_num_params = size(pvp_params_hash, 1);
       

  while(~feof(pvp_template_fid))
    pvp_template_str = fgets(pvp_template_fid);
    pvp_params_str = pvp_template_str;
    for pvp_params_ndx = 1 : pvp_num_params
      pvp_str_ndx = ...
	  strfind(pvp_template_str, ...
		  [pvp_params_token_left, ...
		   pvp_params_hash{pvp_params_ndx, 1}, ...
		   pvp_params_token_right]);
      if ~isempty(pvp_str_ndx)
	pvp_hash_len = ...
	    length(pvp_params_hash{pvp_params_ndx, 1}) + ...
	    length(pvp_params_token_left) + ...
	    length(pvp_params_token_right);
	pvp_template_len = ...
	    length(pvp_template_str);
	pvp_prefix = pvp_template_str(1:pvp_str_ndx-1);
	pvp_suffix = pvp_template_str(pvp_str_ndx+pvp_hash_len:pvp_template_len-1);
	pvp_params_str = ...
	    [pvp_prefix, ...
	     pvp_params_hash{pvp_params_ndx, 2}, ...
	     " = ", ...
	     num2str(pvp_params_hash{pvp_params_ndx, 3}), ...
	     pvp_suffix, ";", "\n"];
	break;
      endif
    endfor  %% pvp_params_ndx
    fputs(pvp_params_fid, pvp_params_str);
  endwhile
  fclose(pvp_params_fid);
  fclose(pvp_template_fid);

endfunction %% pvp_makeParams
