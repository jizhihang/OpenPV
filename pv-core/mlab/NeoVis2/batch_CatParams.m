
%% begin definition of the most volitile parameters
FLAVOR_ID = "Test"; %% "Training"; %% 
disp(["FLAVOR_ID = ", FLAVOR_ID]);
target_id = cell(1,2);
target_id{1,1} = "cat"; target_id{1,2} = "nocatdog"; %% 
disp(["target_id = ", target_id{1,1}, ", ", target_id{1,2}]);
pvp_num_ODD_kernels = 2; %%
disp(["num_ODD_kernels = ", num2str(pvp_num_ODD_kernels)]);
%% end definition of the most volitile parameters

%% version_str stores the training or testing run index 
version_ids = [1:1]; %%  
disp(["version_ids = ", mat2str(version_ids)]);
pvp_frame_size = [256, 360]; %%
disp(["frame_size = ", mat2str(pvp_frame_size)]);
pvp_edge_type = "canny"; 
pvp_clique_id = "3way"; %% ""; %% 
num_versions = length(version_ids);
if num_versions > 0
  version_str = cell(num_versions,1);
  for i_version = 1 : num_versions
    version_str{i_version} = num2str(version_ids(i_version), "%3.3i");   
  endfor
else
  num_versions = 1;
  version_str = cell(num_versions,1);
  version_str{1}="";
endif

global PVP_VERBOSE_FLAG
PVP_VERBOSE_FLAG = 0;

global pvp_home_path
global pvp_workspace_path
global pvp_mlab_path
global pvp_clique_path
pvp_home_path = ...
    [filesep, "home", filesep, "gkenyon", filesep];
if ismac
  pvp_workspace_path = ...
      [pvp_home_path, "workspace", filesep];
else
  pvp_workspace_path = ...
      [pvp_home_path, "workspace", filesep];
endif
pvp_mlab_path = ...
    [pvp_workspace_path, "PetaVision", filesep, "mlab", filesep];
pvp_clique_path = ...
    [pvp_workspace_path, "SynthCog3", filesep];


DATASET_ID = "CatVsNoCatDog";
dataset_id = tolower(DATASET_ID); %% 
flavor_id = tolower(FLAVOR_ID); %% 

pvp_repo_path = ...
    [filesep, "nh", filesep, "compneuro", filesep, "Data", filesep, "ImageNet", filesep];

pvp_petavision_path = ...
    [pvp_repo_path, "PetaVision", filesep]; %%, ...
mkdir(pvp_petavision_path);

pvp_dataset_path = ...
    [pvp_petavision_path, ...
     DATASET_ID, filesep]; %%, ...
mkdir(pvp_dataset_path);

pvp_flavor_path = ...
    [pvp_dataset_path, ...
     FLAVOR_ID, filesep]; %%, ...
mkdir(pvp_flavor_path);

pvp_program_path = ...
    pvp_flavor_path;
mkdir(pvp_program_path);

pvp_input_path2 = ...
    [pvp_clique_path, "input", filesep]; %%, ...
pvp_input_path3 = ...
    [pvp_input_path2, ...
     DATASET_ID, filesep]; %%, ...
mkdir(pvp_input_path3);
pvp_input_path = ...
    [pvp_input_path3, ...
     FLAVOR_ID, filesep];
mkdir(pvp_input_path);

file_of_weights_path = [];
%%if strcmp(FLAVOR_ID, "Test")
  pvp_file_of_weights_path2 = ...
      [pvp_clique_path, "weights", filesep]; %%, ...
  mkdir(pvp_file_of_weights_path2);
  pvp_file_of_weights_path3 = ...
      [pvp_file_of_weights_path2, ...
       DATASET_ID, filesep]; %%, ...
  mkdir(pvp_file_of_weights_path3);
  pvp_file_of_weights_path = ...
      [pvp_file_of_weights_path3, ...
       FLAVOR_ID, filesep];
  mkdir(pvp_file_of_weights_path);
%%endif

pvp_num_ODD_kernels_str = "";
if pvp_num_ODD_kernels > 1
  pvp_num_ODD_kernels_str = num2str(pvp_num_ODD_kernels);
endif
pvp_bootstrap_str = ""; %% "_bootstrap0"; %%  
pvp_num_frames =  []; %% ceil(12294 / num_versions); %%625;

output_activity_path = ...
    [pvp_program_path, ...
     "activity", filesep];
mkdir(output_activity_path);

pvp_list_path = ...
    [pvp_repo_path, ...
     "list", filesep];

checkpoints_path = ...
    [pvp_program_path, ...
     "checkpoints", filesep];
mkdir(checkpoints_path);


%% path to generic image processing routines
util_dir = [pvp_mlab_path, "util", filesep];
addpath(util_dir);
str_dir = [pvp_mlab_path, "stringKernels", filesep];
addpath(str_dir);

checkpoint_read_path = ...
    [pvp_petavision_path, ...
     DATASET_ID, filesep, ...
     "Training", filesep, ...
     "checkpoints", filesep];

for i_object = 1 : size(target_id,1)

  %% get checkpoint read dir and list of weights
  max_checkpoint_dir = cell(num_versions,2);
  %%keyboard;
  pvp_weight_pathname = [];
  pvp_file_of_weights_file = cell(1,2);
  num_weight_files = zeros(1,2);
  for target_flag = 1:2
    disp(target_id{i_object, target_flag});
    
    checkpoint_read_object_path2 = ...
	[checkpoint_read_path, ...
	 target_id{i_object, target_flag}, pvp_num_ODD_kernels_str, filesep];
    checkpoint_read_object_path = ...
	[checkpoint_read_object_path2, ...
	 pvp_edge_type, pvp_clique_id, filesep];
    
    file_of_weights_object_path2 = ...
	[pvp_file_of_weights_path, ...
	 target_id{i_object, target_flag}, pvp_num_ODD_kernels_str, filesep];
    mkdir(file_of_weights_object_path2);
    file_of_weights_object_path = ...
	[file_of_weights_object_path2, ...
	 pvp_edge_type, pvp_clique_id, filesep];
    mkdir(file_of_weights_object_path);
    pvp_file_of_weights_filename = ...
	[DATASET_ID, ...
	 "_", ...
	 FLAVOR_ID, ...
	 "_", ...
	 target_id{i_object, target_flag}, ...
	 pvp_num_ODD_kernels_str, ...
	 "_", ...
	 pvp_edge_type, pvp_clique_id, ...
	 ".weights"];	
    pvp_file_of_weights_file{1, target_flag} = ...
	[pvp_file_of_weights_path, pvp_file_of_weights_filename];
    pvp_file_of_weights_fid = ...
	fopen(pvp_file_of_weights_file{1, target_flag}, "w");
    if pvp_file_of_weights_fid < 0
      disp(["fopen failed: ", pvp_file_of_weights_file{1, target_flag}]);
      return;
    else
      disp(["fopen success: ", pvp_file_of_weights_file{1, target_flag}]);
    endif
    num_weight_files(target_flag) = 0;
    for i_version = 1 : num_versions
      if num_versions > 1
	checkpoint_read_version_path = ...
	    [checkpoint_read_object_path, ...
	     version_str{i_version}, filesep];
      else
	checkpoint_read_version_path = ...
	    checkpoint_read_object_path;
      endif
      disp(["checkpoint_read_version_path = ", checkpoint_read_version_path]);
      checkpoint_dirs = glob([checkpoint_read_version_path, "Checkpoint*"]);
      num_checkpoints = length(checkpoint_dirs);
      max_checkpoint_ndx = 0;
      pvp_weight_pathname = [];
      for i_checkpoint = 1:num_checkpoints
	checkpoint_files = readdir(checkpoint_dirs{i_checkpoint});
	if length(checkpoint_files) < 25
	  continue;
	endif
	checkpoint_folder = strFolderFromPath(checkpoint_dirs{i_checkpoint});
	checkpoint_ndx = str2num(checkpoint_folder(11:end));
	weight_file_path = ...
	    [checkpoint_read_version_path, "Checkpoint", num2str(checkpoint_ndx), filesep];
	disp(["weight_file_path = ", weight_file_path]);
	time_info_pathname = [weight_file_path, "timeinfo.txt"];
	if ~exist(time_info_pathname, "file")
	  continue;
	endif
	if target_flag >= 1
	  if pvp_num_ODD_kernels == 1
	    weight_filename = ...
		"L1Pool1X1toL1Post_W.pvp";
	  elseif pvp_num_ODD_kernels == 2
	    weight_filename = ...
		"L2Pool2X2toL2Post_W.pvp";
	  elseif pvp_num_ODD_kernels == 3
	    weight_filename = ...
		"L3Pool4X4toL3Post_W.pvp";
	  else
	    error(["weight_filename unspecified for pvp_num_ODD_kernels = ", ...
		   num2str(pvp_num_ODD_kernels)]);
	  endif
	else
	  if pvp_num_ODD_kernels == 1
	    weight_filename = ...
		"L1Pool1X1toL1Pool1X1_W.pvp";
	  elseif pvp_num_ODD_kernels == 2
	    weight_filename = ...
		"L2Pool2X2toL2Pool2X2_W.pvp";
	  elseif pvp_num_ODD_kernels == 3
	    weight_filename = ...
		"L3Pool4X4toL3Pool4X4_W.pvp";
	  else
	    error(["weight_filename unspecified for pvp_num_ODD_kernels = ", ...
		   num2str(pvp_num_ODD_kernels)]);
	  endif
	endif
	weight_pathname = [weight_file_path, weight_filename];
	if ~exist(weight_pathname, "file")
	  disp(["~exist(weight_pathname): ", weight_pathname]);
	  %%keyboard;
	  continue;
	endif
	[INFO, ERR, MSG] = stat(weight_pathname);
	if INFO.size < 10^6
	  disp(["file size < 1,000,000: ", weight_pathname]);
	  %%keyboard;
	  continue;
	endif	    
	max_checkpoint_ndx = max([checkpoint_ndx; max_checkpoint_ndx]);
	if checkpoint_ndx == max_checkpoint_ndx
	  pvp_weight_pathname = [weight_pathname, "\n"];
	  max_checkpoint_dir{i_version, target_flag} = weight_file_path;
	endif
      endfor  %% i_checkpoint
      if ~isempty(pvp_weight_pathname)
	fputs(pvp_file_of_weights_fid, pvp_weight_pathname);
	num_weight_files(target_flag) = num_weight_files(target_flag) + 1;
      endif
    endfor  %% i_version
    fclose(pvp_file_of_weights_fid);
  endfor %% target_flag
  
  for target_flag = 1:2   
    pvp_params_template = ...
	[pvp_clique_path, ...
	 "templates", filesep, ...
	 DATASET_ID, filesep, ...
	 FLAVOR_ID, filesep, ...
	 target_id{i_object, target_flag}, pvp_num_ODD_kernels_str, filesep, ...
	 pvp_edge_type, pvp_clique_id, filesep, ...
	 DATASET_ID, "_", FLAVOR_ID, "_", target_id{i_object, target_flag}, pvp_num_ODD_kernels_str, "_", pvp_edge_type, pvp_clique_id, ".", ...
	 "template"];
    
    output_object_path2 = ...
	[output_activity_path, ...
	 target_id{i_object, target_flag}, pvp_num_ODD_kernels_str, filesep];
    mkdir(output_object_path2);
    output_object_path = ...
	[output_object_path2, ...
	 pvp_edge_type, pvp_clique_id, filesep];
    mkdir(output_object_path);
    
    input_object_path2 = ...
	[pvp_input_path, ...
	 target_id{i_object, target_flag}, pvp_num_ODD_kernels_str, filesep];
    mkdir(input_object_path2);
    input_object_path = ...
	[input_object_path2, ...
	 pvp_edge_type, pvp_clique_id, filesep];
    mkdir(input_object_path);
    
    if strcmp(flavor_id, "training")
      list_object_path = ...
	  [pvp_list_path, ...
      	   target_id{i_object, 1}, filesep];
    else
      list_object_path = ...
	  [pvp_list_path, ...
	   target_id{i_object, target_flag}, filesep];
    endif
    list_path = list_object_path;
    
    pvp_fileOfFrames_path = ...
	[list_object_path];
    pvp_fileOfFrames_file = ...
	[flavor_id, "_", "fileOfFilenames1.txt"];
    pvp_fileOfFrames = ...
	[pvp_fileOfFrames_path, pvp_fileOfFrames_file];
    pvp_num_frames = linecount(pvp_fileOfFrames);
    if pvp_num_frames == 0
      error(["linecount = 0:", "pvp_fileOfFrames = ", pvp_fileOfFrames]);
    endif

    if target_flag == 1
      pvp_fileOfMasks_file = ...
	  [flavor_id, "_", "fileOfMasknames1.txt"];
      pvp_fileOfMasks = ...
	  [pvp_fileOfFrames_path, pvp_fileOfMasks_file];
    else
      pvp_fileOfMasks_file = ...
	  ["anti", "_", "fileOfFilenames1.txt"];
      pvp_fileOfMasks = ...
	  [pvp_fileOfFrames_path, pvp_fileOfMasks_file];
    endif
    
    checkpoint_object_path2 = ...
	[checkpoints_path, ...
	 target_id{i_object, target_flag}, pvp_num_ODD_kernels_str, filesep];
    mkdir(checkpoint_object_path2);
    checkpoint_object_path = ...
	[checkpoint_object_path2, ...
	 pvp_edge_type, pvp_clique_id, filesep];
    mkdir(checkpoint_object_path);
    
    
    for i_version = 1 : num_versions
      if num_versions > 1
	disp(["version_str = ", version_str{i_version}]);
	
	output_version_path = ...
	    [output_object_path, ...
	     version_str{i_version}, filesep];
	mkdir(output_version_path);
	output_path = output_version_path;

	checkpoint_read_path = max_checkpoint_dir{i_version, target_flag};
	
	checkpoint_version_path = ...
	    [checkpoint_object_path, ...
	     version_str{i_version}, filesep];
	mkdir(checkpoint_version_path);
	checkpoint_write_path = checkpoint_version_path;
	
	
	input_version_path = ...
	    [input_object_path, ...
	     version_str{i_version}, filesep];
	mkdir(input_version_path);
	input_path = input_version_path;
	
	params_filename = ...
	    [DATASET_ID, ...
	     "_", ...
	     FLAVOR_ID, ...
	     "_", ...
	     target_id{i_object, target_flag}, ...
	     pvp_num_ODD_kernels_str, pvp_bootstrap_str, ...
	     "_", ...
	     pvp_edge_type, pvp_clique_id, ...
	     "_", ...
	     version_str{i_version}, ...
	     ".params"];
	
      
      else
	output_path = [output_object_path];
	
	checkpoint_read_path = max_checkpoint_dir{1, target_flag};
	
	checkpoint_write_path = [checkpoint_object_path];
	
	input_version_path = input_object_path;
	input_path = input_version_path;
	
	params_filename = ...
	    [DATASET_ID, ...
	     "_", ...
	     FLAVOR_ID, ...
	     "_", ...
	     target_id{i_object, target_flag}, ...
	     pvp_num_ODD_kernels_str, pvp_bootstrap_str, ...
	     "_", ...
	     pvp_edge_type, pvp_clique_id, ...
 	     ".params"];
      endif  %% num_versions > 0
        
      disp(["params_filename = ", params_filename]);
      disp(["input_path = ", input_path]);
      disp(["output_path = ", output_path]);
      disp(["checkpoint_read_path = ", checkpoint_read_path]);
      disp(["checkpoint_write_path = ", checkpoint_write_path]);
      [pvp_params_file] = ...
	  pvp_makeCatParams(DATASET_ID, ...
			    FLAVOR_ID, ...
			    pvp_repo_path, ...
			    pvp_program_path, ...
			    target_id{i_object, target_flag}, ...
			    target_flag, ...
			    pvp_num_ODD_kernels, ...
			    pvp_bootstrap_str, ...
			    pvp_edge_type, ...
			    pvp_clique_id, ...
			    version_str{i_version}, ...
			    num_versions, ...
			    input_path, ...
			    num_weight_files, ...
			    pvp_file_of_weights_file, ...
			    pvp_params_template, ...
			    pvp_frame_size, ...
			    pvp_num_frames, ...
			    list_path, ...
			    pvp_fileOfFrames, ...
			    pvp_fileOfMasks, ...
			    output_path, ...
			    checkpoint_read_path, ...
			    checkpoint_write_path, ...
			    params_filename);
    
    endfor %% i_version
  endfor %% target_flag  
endfor  %% i_object


